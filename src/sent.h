#ifndef _SENTENCE2VEC_SENT_H_
#define _SENTENCE2VEC_SENT_H_

#include <iostream>
#include <fstream>
#include <string>
#include <climits>
#include <map>
#include "utils.h"
using namespace std;

namespace sent2vec {
//

class Sent {


public:
    typedef unsigned int IndexType;
    typedef map<string, IndexType> map_type;
    typedef map<string, IndexType>::iterator msit;
    typedef Vec vec_type;
    typedef vec_type::iterator vec_iter;

    Sent(): lenVec(50), curIndex(0) { };

    void init(int lenVec) { this->lenVec = lenVec; }

    Sent(int lenVec): lenVec(lenVec), curIndex(0) { };

    // init from plain text
    void initFromFile(costr path)
    {
        cout << "init sent map from path: " << path << endl;
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
            add(sent);
        }
        infile.close();
    }

    IndexType index(costr sent)
    {
        msit it = dic.find(sent);
        if(it != dic.end())
        {
            return it->second;
        }
        return maxIndex;
    }


    void add(costr sent)
    {
        string trimedSent = trim(sent); 
        if (dic.find(trimedSent) == dic.end())
        {
            dic.insert(
                map<string, IndexType>::value_type(trimedSent, curIndex));
            curIndex ++;
        }
    }
    

    bool contains(costr sent)
    {
        string trimedSent = trim(sent); 
        return dic.find(trimedSent) != dic.end();
        
    }

    // update an Element's vec
    void updateVec(IndexType id, Vec &grad, float alpha)
    {
        vecs[id] += (grad * alpha);
        vecs[id].norm();
    }

    void initVecs()
    {
        cout << "!create sent map: " << dic.size() << endl;
        vecs.clear();
        for (IndexType i=0; i<dic.size(); ++i)
        {
            vector<ItemType> _vec;
            genNormRandVec(lenVec, _vec);
            Vec vec(_vec);
            vecs.push_back(vec);
        }
    }

    Vec toVec(costr key)
    {
        if (vecs.empty())
        {
            initVecs();
        }
        msit it = dic.find(key); 
        if (it != dic.end())
        {
            IndexType idx = it->second;
            return vecs[idx];
        }
    }

    Vec & operator[] (costr key)
    {
        if (vecs.empty())
        {
            initVecs();
        }
        msit it = dic.find(key); 

        assert(it != dic.end());

        if (it != dic.end())
        {
            IndexType idx = it->second;
            return vecs[idx];
        }
    }

    IndexType size() const
    {
        return dic.size();
    }

protected:
    int lenVec;
    IndexType curIndex;
    map<string, IndexType> dic;
    vector< Vec > vecs;
};

//
}; // end namespace



#endif
