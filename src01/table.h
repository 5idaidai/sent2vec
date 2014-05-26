#ifndef _SENTENCE2VEC_TABLE_H_
#define _SENTENCE2VEC_TABLE_H_
#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <climits>
#include <cmath>
#include "utils.h"
#include "vocab.h"
using namespace std;

namespace sent2vec {

const IndexType RAND_TABLE_SIZE = 200000;



/*
 * accelerate multi-thread program
 */
class RandTable {

public:
    RandTable() {
        // init current status
        curRand = 21;
        // init random table
        for(int i=0; i<RAND_TABLE_SIZE; i++) {
            IndexType randValue = rand() % TABLE_SIZE;
            rands[i] = randValue;
        }
    }

    IndexType getRand() {
        curRand = rands[curRand % RAND_TABLE_SIZE];
    }

private:
    IndexType rands[RAND_TABLE_SIZE];
    IndexType curRand;
};
/*
 * a word sample table 
 */
class WordTable {

public:
    typedef map<string, IndexType> dicType ;

    WindowTable():vocab_p(NULL), TABLE_SIZE(1e7), POWER(0.75){}

    WindowTable(Vocab *vocab_p, costr path, 
        IndexType TABLE_SIZE=1e7, 
        float POWER=0.75): 
        vocab_p(vocab_p), path(path), 
        TABLE_SIZE(TABLE_SIZE), POWER(POWER){
            init(vocab_p, path);
    }

    void init(Vocab *vocab_p, costr path) {
        this->vocab_p = vocab_p;
        this->path = path;
        // create word_count_table
        map<string, int> wordCountDic;
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
            vector<string> words;
            split(sent, words, " ");
            for(vsit it=words.begin(); it!=words.end(); ++it)
            {
                map<string, int>::iterator res = wordCountDic.find(*it);
                if (res == wordCountDic.end()) {
                    wordCountDic.insert(
                        map<string, int>::value_type(*it, 1));
                } else {
                    res->second ++;
                }
            }
        }
        // to index vec
        for(map<string, IndexType>::iterator it=wordCountDic.begin(); it!=wordCountDic.end(); ++it) {
            IndexType idx = vocab.index(it->first);
            assert(idx != maxIndex);
            idxCountMap.push_back(
                map<IndexType, int>::value_type(
                    idx, it->second));
        }
    }

    const vector<IndexType> &genTable() {
        float trainPowSum = 0.0;
        vector<string> 

        for(map<IndexType, int>::iterator it=idxCountMap.begin(); it!=idxCountMap.end(); ++it) {
            trainPowSum += pow(it->second, POWER);
        }

        IndexType i = 0;
        float d1 = pow(
            (float)(idxCountMap[i].second) / trainWindowsPowSum, POWER);

        for(IndexType a=0; a<TABLE_SIZE; ++a) {
            table[a] = i;
            if(a * 1.0 / TABLE_SIZE > d1) {
                i++;
                d1 += pow(
                    (float)(idxCountMap[i].second) / trainWindowsPowSum, POWER)
                if (i >= vocabSize) i = vocabSize - 1;
            } // if
        } // for
        return table;
    }

    vector<IndexType> getSamples(int k) {
        if(table.empty) genTable();
        vector<IndexType> samples;
        for(int i=0; i<k; i++) {
            IndexType item = randTable.getRand();
            samples.push_back(item);
        }
        return samples;
    }

private:
    RandTable randTable;
    map<IndexType, int> idxCountMap;
    vector<IndexType> table;
    string path;

}; // class WordTable


}; // namespace sent2vec







#endif
