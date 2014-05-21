#include <iostream>
#include <cmath>
#include <string>
#include <pthread.h>
#include "utils.h"
#include "sent.h"
#include "vocab.h"
#include "windowtable.h"
#include "../wqueue/wqueue.h"
#include "../threads/thread.h"
using namespace std;
using namespace sent2vec;

// shared data ----------------------
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

// record  costs
class Results {
public:
    Results() {
        pthread_mutex_init(&mutex, NULL);
    }

    void append(float Jn) {
        pthread_mutex_lock(&mutex);
        results.append(Jn);
        pthread_mutex_unlock(&mutex);
    }

    void show() {
        results.show();
    }

    int size() {
        return results.size();
    }

    bool empty() {
        return results.empty();
    }

    void clear() {
        results.clear();
    }

    float mean() {
        return results.mean();
    }

private:
    Vec results;
    pthread_mutex_t mutex;
};

Results results;

/*
 * read senences from file and put them to WorkQueue
 */

class Producer : public Thread {

public:
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
};


class Trainer : public Thread {
public:
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
            //cout << "v : " << endl;
            //v.show();

            Vec updateV(lenVec);

            float Jn = 0.0;

            for (vector<vector<string> >::iterator wt=windows.begin(); wt!=windows.end(); ++wt)
            {
                // positive  -----------
                // window vector
                string window_key = genWindowKey(vocab, *wt);
                Vec h = vocab.getWindowVec(window_key);
                //cout << "h: " << endl;
                //h.show();
                float e_h_v = exp(h.dot(v));
                //cout << "e_h_v " << e_h_v << endl;
                Vec partial_J_h = v * (1.0 / (1.0 + e_h_v));
                Vec partial_J_v = h * (1.0 / (1.0 + e_h_v));
                updateV += partial_J_v;
                Jn += log(1.0 / (1.0 + e_h_v));
                // update vector
                vocab.updateWindow(window_key, partial_J_h, alpha);
                // add noises
                vector<string> noises = windowTable.getSamples(k);
                for(vector<string>::iterator wt=noises.begin(); wt!=noises.end(); ++wt)
                {
                    Vec h = vocab.getWindowVec(*wt);
                    float e_h_v = exp(h.dot(v));
                    Vec partial_J_h = v * (e_h_v / (1.0 + e_h_v));
                    Vec partial_J_v = h * (e_h_v / (1.0 + e_h_v));
                    Jn += log(1.0 / (1.0 + e_h_v));
                    updateV += partial_J_v;
                    // update vectors
                    vocab.updateWindow(*wt, partial_J_h, alpha);
                }

                updateV /= (1+ k);
                sent.updateVec(sent_id, updateV, alpha);
            }
            // output result
            results.append(Jn);
        }
        return NULL;
    }
};


class Sent2Vec {
public:
    Sent2Vec(
            costr ipath, 
            int iwindowSize=2, 
            int inThreads = 2, 
            float ialpha = 0.1, 
            int ik = 20) {
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
            cout << "J: " << J << endl;
            //cout << "size: " << results.size() << endl;
            results.clear();
        }
        //cout << "main done!" << endl;
    }

    ~Sent2Vec() {
    }

private:
    Producer *producer;
    vector<Trainer*> threads;
};


int main()
{
    Sent2Vec sent2vec("1.sample", 2, 3);
    sent2vec.initData();
    for (int i=0; i<20; i++) {
        cout << i << endl;
        sent2vec.run();
    }
    printf("main done!");
    return 0;
}
