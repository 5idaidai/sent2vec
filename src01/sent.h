#ifndef _SENTENCE2VEC_SENT_H_
#define _SENTENCE2VEC_SENT_H_

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <climits>
#include <map>
#include <pthread.h>
#include "utils.h"
using namespace std;

#define USE_MUTEX_LOCK

namespace sent2vec {
//

class Sent {


public:
    typedef unsigned int IndexType;
    typedef map<string, IndexType> map_type;
    typedef map<string, IndexType>::iterator msit;
    typedef Vec vec_type;
    typedef vec_type::iterator vec_iter;
    // for iterator
    typedef map<string, IndexType>::iterator iterator;
    typedef map<string, IndexType>::const_iterator const_iterator;


    Sent(): lenVec(50), curIndex(0) {
    #ifdef USE_MUTEX_LOCK
        pthread_mutex_init(&m_mutex, NULL);
    #else
        pthread_spin_init(&m_mutex, 0);
    #endif
    };

    void init(int lenVec, bool isPthread=false) { this->lenVec = lenVec; this->isPthread=isPthread; }

    Sent(int lenVec): lenVec(lenVec), curIndex(0) { 
    #ifdef USE_MUTEX_LOCK
        pthread_mutex_init(&m_mutex, NULL);
    #else
        pthread_spin_init(&m_mutex, 0);
    #endif
    };

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
    #ifdef USE_MUTEX_LOCK
        pthread_mutex_lock(&m_mutex);
    #else
        pthread_spin_lock(&m_mutex);
    #endif
        
        vecs[id] -= (grad * alpha);
        vecs[id].norm();
    #ifdef USE_MUTEX_LOCK
        pthread_mutex_unlock(&m_mutex);
    #else
        pthread_spin_unlock(&m_mutex);
    #endif
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
            //vec.show();
            vecs.push_back(vec);
        }
    }

    void initVecs(costr path)
    {
        cout << "init random from file: " << path << endl;
        ifstream infile(path.c_str());
        if(!infile)
        {
            cout << "*ERROR: no such file :" << path << endl;
            exit(-1);
        }
        vecs.clear();
        string line;
        for (IndexType i=0; i<dic.size(); ++i)
        {
            getline(infile, line);
            if (line.empty()) 
            {
                cout << "ERROR: random init file is not long enough" << endl;
            }
            vector<string> words;
            line = trim(line);
            split(line, words, " ");
            assert(words.size() == lenVec);

            Vec vec(lenVec);
            for (IndexType j=0; j<lenVec; j++)
            {
                vec[j] = atof(words[j].c_str());
            }
            vecs.push_back(vec);
        }
        infile.close();
    }

    Vec toVec(costr key)
    {
        msit it = dic.find(key); 
        if (it != dic.end())
        {
            IndexType idx = it->second;
            return vecs[idx];
        }
    }

    Vec & operator[] (costr key)
    {
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

    // model parameters to file
    void tofile(costr path) {
        cout << "output Sent to file: " << path << endl;
        const string SPACE = "\t";
        stringstream output;
        // output dic
        for (msit it=dic.begin(); it!=dic.end(); ++it) {
            output << it->first << SPACE << it->second << endl;
        }
        // output vecs
        for(vector<Vec>::iterator it=vecs.begin(); it!=vecs.end(); ++it) {
            for(Vec::iterator vt=it->begin(); vt!=it->end(); ++vt) {
                output << *vt << SPACE;
            }
            output << endl;
        }
        // write to file
        ofstream file(path.c_str(), ios::out);
        if(! file) {
            cout << "ERROR, can not open file: " << path << endl;
            exit(-1);
        }
        file << output.str();
        file.close();
    }

    void fromfile(costr path) {
    }

    iterator begin() {
        return dic.begin();
    }

    iterator end() {
        return dic.end();
    }



protected:
    int lenVec;
    IndexType curIndex;
    map<string, IndexType> dic;
    vector< Vec > vecs;
    bool isPthread;
    // for vectors change

#ifdef USE_MUTEX_LOCK
    pthread_mutex_t  m_mutex;
#else
    pthread_spinlock_t m_mutex;
#endif
};

//
}; // end namespace



#endif
