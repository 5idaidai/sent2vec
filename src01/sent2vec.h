#include <iostream>
#include <cmath>
#include <string>
#include <pthread.h>
#include "utils.h"
#include "sent.h"
#include "vocab.h"
#include "table.h"
#include "../wqueue/wqueue.h"
#include "../threads/thread.h"

namespace sent2vec {


vector<vstr>  genWindowsFromSentence(costr sent, int size)
{
    vstr words; 
    split(sent, words, " ");
    vector< vstr > windows;
    // skip sentences too short
    if(words.size() < size) 
    {
        // cout << "skip sentence:" << sent << endl;
        return windows;
    }

    for (int i=0; i<words.size() - size; i++)
    {
        vector<string> window; 
        window.assign(words.begin()+i, words.begin()+i+size);
        windows.push_back(window);
    }
    return windows;
}



const string STOP_MARK = "~!@#$%";

class Producer : public Thread {

public:
    Producer(costr path, wqueue<string> *workQueue_p, int nThreads) : workQueue_p(workQueue_p) , path(path), nThreads(nThreads){}

    void *run() {
        ifstream infile(path.c_str());
        if(!infile)
        {
            cout << "*ERROR: no such file :" << path << endl;
            exit(-1);
        }
        string sentence;
        while(getline(infile, sentence))
        {
            sentence = trim(sentence);
            workQueue_p->add(sentence);
        }
        for(int i=0; i<2*nThreads; ++i)
        {
            workQueue_p->add(STOP_MARK);
        }
        infile.close();
        return NULL;
    }
private:
    wqueue<string> *workQueue_p;
    string path;
    int nThreads;
};


class Trainer : public Thread {

public:

    Trainer(Sent *sent_p, Vocab *vocab_p, WordTable* wordTable_p, wqueue<string>* workQueue_p, Results *results_p, int k, float alpha, int windowSize) : sent_p(sent_p), vocab_p(vocab_p), wordTable_p(wordTable_p), k(k), alpha(alpha) , windowSize(windowSize), workQueue_p(workQueue_p), results_p(results_p){}

    void *run() {
        while(true) {
            string sentence = workQueue_p->remove();
            
            if (sentence == STOP_MARK) break;
            // train by sentence
            vector<vstr> windows = genWindowsFromSentence(sentence, windowSize);
            if(windows.empty()) continue;
            IndexType sent_id = sent_p->index(sentence);
            // sentence vector
            Vec v = (*sent_p)[sentence];

            Vec updateV(lenVec);

            float Jn = 0.0;

            for (vector<vector<string> >::iterator wt=windows.begin(); wt!=windows.end(); ++wt)
            {
                Vec w = getContext(*wt);
                // right sum ? 
                positiveTrain(v, w, *wt, updateV, Jn);
                // train negative samples
                negativeTrain(v, w, *wt, updateV, Jn);
                updateV /= (1.0 + this->k);
                sent_p->updateVec(sent_id, updateV, alpha);
            }
            //cout << "Jn: " << Jn << endl;
            results_p->append(Jn);
        }
    }
    /*
     * v : sentence vector
     * w : context
     */
    void positiveTrain(Vec &v, Vec &w, vector<string>&window, Vec &updateV, float &Jn) {
        // get current word's vector
        IndexType curWordId;
        Vec t(lenVec);
        getCurrentWord(window, curWordId, t);
        // calculate the partial
        Vec h = w; h += t; h /= windowSize;
        float e_h_v = exp(h.dot(v));
        Vec partial_J_h = v * (1.0 / (1.0 + e_h_v));
        Vec partial_J_v = h * (1.0 / (1.0 + e_h_v));
        updateV += partial_J_v;
        Jn += log(1.0 / (1.0 + 1.0/e_h_v));
        // update context and current word's vector
        updateContext(window, partial_J_h);
        vocab_p->updateVec(curWordId, partial_J_h, alpha);
    }
    /*
     * train with negative samples
     */
    void negativeTrain(Vec &v, Vec &w, vector<string>&window, Vec &updateV, float &Jn) {
        vector<IndexType> noises = wordTable_p->getSamples(k);
        for(int i=0; i<k; i++) {
            IndexType wordId = noises[i];
            //cout << "wordId " << wordId << endl;
            Vec t = (*vocab_p)[wordId];
            Vec h = w; h += t; h /= windowSize;
            float e_h_v = exp(h.dot(v));
            Vec partial_J_h = v * (e_h_v / (1.0 + e_h_v));
            Vec partial_J_v = h * (e_h_v / (1.0 + e_h_v));
            Jn += log(1.0 / (1.0 + e_h_v));
            updateV += partial_J_v;
            // update vectors
            updateContext(window, partial_J_h);
            vocab_p->updateVec(wordId, partial_J_h, alpha);
        }
    }
    /* 
     * pass a window
     * treat all words but the last one as the context
     */
    Vec getContext(vector<string> &window) {
        Vec vec(lenVec);
        for(int i=0; i!= windowSize-1; i++) {
            //cout << "get: " << window[i] << endl;
            vec += (*vocab_p)[window[i]];
        }
        return vec;
    }

    /*
     * pass a window
     * treat the last word as the one to predict
     */
    bool getCurrentWord(vector<string> &window, IndexType &wordId, Vec &vec) {
        string lastWord = window[windowSize-1];
        wordId = vocab_p->index(lastWord);
        vec = (*vocab_p)[lastWord];
        return wordId == maxIndex;
    }
    /*
     * just update first k-1 words(context) 
     */
    void updateContext(vector<string> &window, Vec &partial_J_h) {
        for(int i=0; i<windowSize-1; i++) {
            IndexType id = vocab_p->index(window[i]);
            vocab_p->updateVec(id, partial_J_h, alpha);
        }
    }

private:
    Sent *sent_p;
    Vocab *vocab_p;
    wqueue<string> *workQueue_p;
    WordTable *wordTable_p;
    Results *results_p;
    int k;
    float alpha;
    int windowSize;
};



class Sent2Vec {
public:
    Sent2Vec(
            costr ipath, 
            int iwindowSize=2, 
            int inThreads = 2, 
            float ialpha = 0.1, 
            int ik = 20, float convergence=0.01) : convergence(convergence) {
        // init data
        path = ipath;
        windowSize = iwindowSize;
        alpha = ialpha;
        nThreads = inThreads;
        k = ik;
    }

    void initData() {
        vocab.init(lenVec);
        sent.init(lenVec);
        vocab.initFromFile(path);
        sent.initFromFile(path);
        vocab.initVecs("rand.txt");
        sent.initVecs("rand.txt");
        wordTable.init(&vocab, path);
        wordTable.genTable();
    }

    bool isConverge() {
        int size = Js.size();
        // at least 8 turns
        if(size > 8) {
            if(Js[size-1] - Js[size-2] < convergence) return true;
        }
        return false;
    }

    void run() {
        //cout << "start producer" << endl;
        producer = new Producer(path, &workQueue, nThreads);
        producer->start();

        for (int i=0; i<nThreads; ++i) {
            //cout << "start trainer " << i << endl;
            Trainer *trainer = new Trainer(&sent, &vocab, &wordTable, &workQueue, &results, k, alpha, windowSize);
            threads.push_back(trainer);
            trainer->start();
        }
        // producer.join();
        producer->join();
        for(int i=0; i<nThreads; ++i) {
            //cout << "threads " << i << " join" << endl;
            (threads[i])->join();
        }
        // clear memory
        delete producer;
        producer = NULL;
        for(int i=0; i<nThreads; ++i) {
            delete threads[i];
        }
        threads.clear();

        // output cost
        if( ! results.empty())
        {
            float J = results.mean();
            Js.append(J);
            cout << "J: " << J << endl;
            results.clear();
        }
        //cout << "main done!" << endl;
    }

    ~Sent2Vec() {
    }

private:
    // threads
    Producer *producer;
    vector<Trainer*> threads;
    // data
    float convergence;
    Sent sent;      // sent map class
    Vocab vocab;    // vocab map class
    WordTable wordTable;    // windows
    float alpha;
    int nThreads;
    int windowSize;
    string path;    // data's path
    int k;
    wqueue<string> workQueue; // save stentences
    // save the cost of each turn
    // used to check converge
    Vec Js;
    Results results;
}; // class sent2vec




}; // namespace sent2vec
