#include <iostream>
#include <cmath>
#include "utils.h"
#include "sent.h"
#include "vocab.h"
#include "windowtable.h"
using namespace std;

namespace sent2vec {
//
class Sent2Vec {
public:
    Sent2Vec(costr path="", 
                int lenVec=50, 
                int windowSize=3, 
                int k=20, float alpha=0.1): \
        path(path), lenVec(lenVec), windowSize(windowSize), k(k), alpha(alpha)
    {
        // init objects
        vocab.init(lenVec);
        sent.init(lenVec);
        windowTable.init(windowSize, &vocab, path);
        if(!path.empty())
        {
            createVocab();
            createSent();
            createWindowTable();
        }
    }

    void createVocab()
    {
        vocab.initFromFile(path);
    }

    void createSent()
    {
        sent.initFromFile(path);
    }

    void createWindowTable()
    {
        windowTable.genTable();
    }

    // train from a file
    void train(costr path)
    {
        ifstream infile(path.c_str());
        if(!infile)
        {
            cout << "*ERROR: no such file :" << path << endl;
            exit(-1);
        }

        string sentence;
        Vec Jns;
        while(getline(infile, sentence))
        {
            sentence = trim(sentence);
            float Jn = trainBySent(sentence);
            if (Jn != 0.0)
            {
                Jns.append(Jn);
            }
        }
        cout << "Jn: " << Jns.mean() << endl;
        Jns.clear();
        infile.close();
    }

    float trainBySent(costr sentence)
    {
        float Jn = 0.0;
        vector<vstr> windows = genWindowsFromSentence(sentence, windowSize);
        if(windows.empty()) return 0.0;
        // pointer to the real vector
        Vec *v_p = &sent[sentence];

        for(int wn=0; wn<windows.size(); ++wn)
        {
            vector<IndexType> wordIdxs;
            // get word's id in vocabulary
            string windowKey = genWindowKey(vocab, windows[wn]);
            Vec h = vocab.getWindowVec(windows[wn]);
            // generate noise samples
            vstr samples = windowTable.getSamples(this->k);
            // for positive sample
            // e^(v^T h)
            cout << "v*h" << v_p->dot(h) << endl;
            float e_vT_h = exp( v_p->dot(h));
            cout << "e_vT_h" << e_vT_h << endl;
            exit(0);
            Vec update_v = h / ( 1.0 + e_vT_h);
            Vec update_h = (*v_p) / (1.0 + e_vT_h);
            Jn += log( 1.0 / ( 1.0 + 1.0/e_vT_h));
            vocab.updateWindow(windowKey, update_h, alpha);

            for(int idx=0; idx<samples.size(); idx++)
            {
                string key = samples[idx];
                // get noise's window vector
                Vec n_h = vocab.getWindowVec(key);
                float e_vT_h = exp(v_p->dot(n_h));
                float frac_e_v_h = 1.0 - 1.0 / (1.0 + e_vT_h);
                update_v -= (n_h * frac_e_v_h);
                Vec update_n_h = (*v_p) * (-1.0 * frac_e_v_h);
                // update noise
                vocab.updateWindow(key, update_n_h, alpha);
                Jn += log( 1.0 / ( 1.0 + e_vT_h));
            }

            update_v /= (1.0 + this->k);

            IndexType sentId = sent.index(sentence);
            sent.updateVec(sentId, update_v, alpha);
        }
        return Jn;
    }


private:
    string path;
    int lenVec;
    int k;
    float alpha;
    Vocab vocab;
    Sent sent;
    int windowSize;
    WindowTable windowTable;
};
//
}; // end namespace

int main()
{
    using namespace sent2vec;
    Sent2Vec s2v("1.sample");
    for(int i=0; i<50; i++)
    {
        s2v.train("1.sample");
    }




    return 0;
}
