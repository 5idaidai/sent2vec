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

    Vocab(int lenVec): Sent(lenVec){ };

    /*
    // get vec by index
    Vec& index(IndexType id)
    {
        if (vecs.empty())
        {
            initVecs();
        }
        return vecs[id];
    }
    */
    // init from plain text
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

    vec_type getWindowVec(vector<string> &words)
    {
        if (vecs.empty())
        {
            initVecs();
        }

        // init to 0
        vec_type vec(lenVec);

        for(vector<string>::iterator 
                it=words.begin();
                it != words.end();
                ++it)
        {
            vec += (*this)[*it];
        }
        return vec;
    }


    vec_type getWindowVec(string wordIdx)
    {
        vector<string> words;
        split(wordIdx, words, "-");

        Vec vec(lenVec);

        for(vector<string>::iterator it=words.begin(); 
                it!=words.end(); it++)
        {
            IndexType id = (IndexType)atoi(it->c_str());
            vec += vecs[id];
        }
        return vec;
    }


}; // end class Vec

//
}; // end namespace



#endif
