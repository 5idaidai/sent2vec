#ifndef _SENTENCE2VEC_VOCAB_H_
#define _SENTENCE2VEC_VOCAB_H_

#include <iostream>
#include <string>
#include <fstream>
#include <climits>
#include <map>
#include "utils.h"
#include "sent.h"
using namespace std;

namespace sent2vec {
//

class Vocab : public Sent {

public:
    Vocab() {}

    void init(int lenVec) { this->lenVec = lenVec; }

    Vocab(int lenVec): Sent(lenVec){ };

    // init wordmap from plain text
    void initFromFile(costr path)
    {
        cout << "init word map from path: " << path << endl;
        ifstream infile(path.c_str());
        if(!infile)
        {
            cout << "*ERROR: no such file :" << path << endl;
            exit(-1);
        }

        string sent;
        while(getline(infile, sent))
        {
            sent = trim(sent);
            addFromSentence(sent);
        }
        infile.close();
        cout << "get words " << dic.size() << endl;
    }

    void addFromSentence(costr sent)
    {
        vector<string> words;
        split(sent, words, " ");
        for(vsit it=words.begin(); it!=words.end(); ++it)
        {
            add(*it);
        }
    }
}; // end class vocab





//
}; // end namespace



#endif
