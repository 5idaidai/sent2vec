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

class Producer : public Thread {

public:
    Producer(wqueue<string> *workQueue_p) : workQueue_p(workQueue_p) {}

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
            workQueue.add(sentence);
        }
        for(int i=0; i<2*nThreads; ++i)
        {
            workQueue.add(STOP_MARK);
        }
        infile.close();
        return NULL;
    }
private:
    wqueue<string> *workQueue_p;
};


class Trainer : public Thread {

public:

    Trainer(Sent *sent_p, Vocab *vocab_p, WordTable* wordTable_p, int k, float alpha, int windowSize) : sent_p(sent_p), vocab_p(vocab_p), wordTable_p(wordTable_p), k(k), alpha(alpha) , windowSize(windowSize){}

    void *run() {
        while(true) {
            string sentence = workQueue.remove();
            
            if (sentence == STOP_MARK) break;
            // train by sentence
            vector<vstr> windows = genWindowsFromSentence(sentence, windowSize);
            if(windows.empty()) continue;
            IndexType sent_id = sent.index(sentence);
            // sentence vector
            Vec v = sent[sentence];

            Vec updateV(lenVec);

            float Jn = 0.0;

            for (vector<vector<string> >::iterator wt=windows.begin(); wt!=windows.end(); ++wt)
            {
                // right sum ? 
                positiveTrain(v, w, *wt, updateV, Jn);
                // train negative samples
                negativeTrain(v, *wt, updateV);
                updateV /= (1 + this->k);
                sent.updateVec(sent_id, updateV, alpha, Jn);
            }
            results_p->append(Jn);
        }
    /*
     * v : sentence vector
     * w : context
     */
    void positiveTrain(Vec &v, Vec &w, vector<string>&window, Vec &updateV, float &Jn) {
        // get current word's vector
        IndexType curWordId;
        Vec t;
        getCurrentWord(*wt, curWordId, t);
        // calculate the partial
        Vec h = w + t; h.norm();
        float e_h_v = exp(h.dot(v));
        Vec partial_J_h = v * (1.0 / (1.0 + e_h_v));
        Vec partial_J_v = h * (1.0 / (1.0 + e_h_v));
        updateV += partial_J_v;
        Jn += log(1.0 / (1.0 + e_h_v));
        updateContext(*wt, partial_J_h);
        vocab_p->updateVec(curWordId, partial_J_h, alpha);
    }
    /*
     * train with negative samples
     */
    void negativeTrain(Vec &v, vector<string>&window, Vec &updateV, float &Jn) {
        vector<IndexType> noises = wordTable_p->getSamples(k);
        for(int i=0; i<k; i++) {
            IndexType wordId = noises[i];
            Vec t = (*vocab_p)[noises[i]];
            Vec h = w + t;
            float e_h_v = exp(h.dot(v));
            Vec partial_J_h = v * (e_h_v / (1.0 + e_h_v));
            Vec partial_J_v = h * (e_h_v / (1.0 + e_h_v));
            Jn += log(1.0 / (1.0 + e_h_v));
            updateV += partial_J_v;
            // update vectors
            updateContext(*wt, partial_J_h, alpha);
            vocab_p->updateVec(wordId, partial_J_h, alpha);
        }
    }
    /* 
     * pass a window
     * treat all words but the last one as the context
     */
    Vec getContext(vector<string> &window) {
        Vec vec(k);
        for(int i=0; i!= k-1; i++) {
            vec += (*vocab_p)[window[i]];
        }
        return vec;
    }

    /*
     * pass a window
     * treat the last word as the one to predict
     */
    bool getCurrentWord(vector<string> &window, IndexType &wordId, Vec &vec) {
        string lastWord = window[k-1];
        wordId = vocab_p->index(lastWord);
        vec = (*vocab_p)[lastWord];
        return wordId == maxIndex;
    }
    /*
     * just update first k-1 words(context) 
     */
    void updateContext(vector<string> &window, partial_J_h) {
        for(int i=0; i<k-1; i++) {
            IndexType id = vocab_p->index(window[i]);
            updateVec(id, partial_J_h, alpha);
        }
    }

private:
    Sent *sent_p;
    Vocab *vocab_p;
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
        windowTable.init(windowSize, &vocab, path);
        vocab.initFromFile(path);
        sent.initFromFile(path);
        vocab.initVecs("rand.txt");
        sent.initVecs("rand.txt");
        windowTable.genTable();
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
        producer = new Producer;
        producer->start();

        for (int i=0; i<nThreads; ++i)
        {
            //cout << "start trainer " << i << endl;
            Trainer *trainer = new Trainer;
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
    WindowTable windowTable;    // windows
    const string STOP_MARK = "~!@#$%";
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
};




}; // namespace sent2vec
