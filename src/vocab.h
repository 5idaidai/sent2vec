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
        initVecs();
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
        vec /= words.size();
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

    void updateWindow(costr windowKey, Vec &grad, float alpha)
    {
        vstr wordIds;
        split(windowKey, wordIds, "-");
        for (vstr::iterator it=wordIds.begin();
                it != wordIds.end(); ++it)
        {
            IndexType id = atoi(it->c_str());
            updateVec(id, grad, alpha);
        }
    }


}; // end class Vec





//
}; // end namespace



#endif
