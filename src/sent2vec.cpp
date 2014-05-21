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
                int windowSize=2, 
                int k=20, float alpha=0.1, string randPath="rand.txt"): \
        path(path), lenVec(lenVec), windowSize(windowSize), k(k), alpha(alpha), randPath(randPath)
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
        vocab.initVecs("rand.txt");
    }

    void createSent()
    {
        sent.initFromFile(path);
        sent.initVecs("rand.txt");
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
        vector<vstr> windows = genWindowsFromSentence(sentence, windowSize);
        if(windows.empty()) return 0.0;
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
            // sent.updateVec(sent_id, partial_J_v, alpha);
            vocab.updateWindow(window_key, partial_J_h, alpha);
            // add noises
            vector<string> noises = windowTable.getSamples(this->k);
            for(vector<string>::iterator wt=noises.begin(); wt!=noises.end(); ++wt)
            {
                //cout << "noise: " << *wt << endl;
                Vec h = vocab.getWindowVec(*wt);
                float e_h_v = exp(h.dot(v));
                //cout << "h*v" << h.dot(v) << endl;
                //cout << "e_h_v" << e_h_v << endl;
                Vec partial_J_h = v * (e_h_v / (1.0 + e_h_v));
                Vec partial_J_v = h * (e_h_v / (1.0 + e_h_v));
                Jn += log(1.0 / (1.0 + e_h_v));
                updateV += partial_J_v;
                // update vectors
                vocab.updateWindow(*wt, partial_J_h, alpha);
            }

            updateV /= (1+ this->k);
            sent.updateVec(sent_id, updateV, alpha);
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
    string randPath;
    WindowTable windowTable;
};
//
}; // end namespace

int main()
{
    srand((unsigned) time(NULL));
    using namespace sent2vec;
    Sent2Vec s2v("1.sample");
    for(int i=0; i<50; i++)
    {
        cout << "i:" << i << endl;
        s2v.train("1.sample");
    }




    return 0;
}
