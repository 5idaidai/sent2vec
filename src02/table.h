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

const IndexType RAND_TABLE_SIZE = 500000;
const IndexType TABLE_SIZE = 1e8;


/*
 * accelerate multi-thread program
 */
class RandTable {

public:
    RandTable()  {
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
        return curRand;
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

    WordTable():vocab_p(NULL), POWER(0.75){}

    WordTable(Vocab *vocab_p, costr path, 
        float POWER=0.75): 
        vocab_p(vocab_p), path(path), POWER(POWER){
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
        for(map<string, int>::iterator it=wordCountDic.begin(); it!=wordCountDic.end(); ++it) {
            IndexType idx = vocab_p->index(it->first);
            assert(idx != maxIndex);
            // index to count
            idxCountMap.insert(
                map<IndexType, int>::value_type(
                    idx, it->second));
        }
    }

    const vector<IndexType> &genTable() {
        float trainPowSum = 0.0;
        int vocabSize = idxCountMap.size();
        // init table 
        table.resize(TABLE_SIZE);
        for(map<IndexType, int>::iterator it=idxCountMap.begin(); it!=idxCountMap.end(); ++it) {
            trainPowSum += pow(it->second, POWER);
        }

        IndexType i = 0;
        float d1 = pow(
            (float)(idxCountMap[i]) / trainPowSum, POWER);

        for(IndexType a=0; a<TABLE_SIZE; ++a) {
            table[a] = i;
            if(a * 1.0 / TABLE_SIZE > d1) {
                i++;
                d1 += pow(
                    (float)(idxCountMap[i]) / trainPowSum, POWER);
                if (i >= vocabSize) i = vocabSize - 1;
            } // if
        } // for
        return table;
    }

    vector<IndexType> getSamples(int k) {
        if(table.empty() ) genTable();
        vector<IndexType> samples;
        for(int i=0; i<k; i++) {
            IndexType randValue = randTable.getRand();
            IndexType item = table[randValue];
            samples.push_back(item);
        }
        return samples;
    }

private:
    RandTable randTable;
    Vocab *vocab_p;
    map<IndexType, int> idxCountMap;
    vector<IndexType> table;
    string path;
    float POWER;

}; // class WordTable


}; // namespace sent2vec







#endif
