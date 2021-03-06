#ifndef _SENTENCE2VEC_UTILS_H_
#define _SENTENCE2VEC_UTILS_H_


#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <climits>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#define NDEBUG
#include <cassert>
#include "pthread.h"
#include <deque>
using namespace std;

namespace sent2vec {

// types
typedef float ItemType;

typedef const string& costr;

typedef unsigned int uint;

typedef unsigned int IndexType;

const int lenVec = 50;

const float E = 2.7183;

const int maxIndex = UINT_MAX;

// vector string
typedef vector<string> vstr;
typedef vector<string>::iterator vsit;

void test(string name)
{
    cout << "----------------------------------------" << endl;
    cout << "> testing " << name << endl;
    cout << "----------------------------------------" << endl;
    cout << endl;
}

int stoi(costr v) {
    return atoi(v.c_str());
}

float stof(costr v ) {
    return atof(v.c_str());
}

template <class T>
string join(const vector<T> &words, costr token)
{
    stringstream output;
    for(int i=0; i<words.size()-1; i++)
    {
        output << words[i] << token;
    }
    output << words.back();
    return output.str();
}

// tools
int split(costr str, vector<string>& ret_, string sep = ",")
{
    if (str.empty())
        return 0;

    string tmp;
    string::size_type pos_begin = str.find_first_not_of(sep);
    string::size_type comma_pos = 0;

    while (pos_begin != string::npos)
    {
        comma_pos = str.find(sep, pos_begin);
        if (comma_pos != string::npos)
        {
            tmp = str.substr(pos_begin, comma_pos-pos_begin);
            pos_begin = comma_pos + sep.length();
        }
        else
        {
            tmp = str.substr(pos_begin);
            pos_begin = comma_pos;
        }

        if(!tmp.empty())
        {
            ret_.push_back(tmp);
            tmp.clear();
        }
    }
    return 0;
}

string trim(costr str)
{
    string tags = " \n\t\r";
    string::size_type pos = str.find_first_not_of(tags);
    if(pos == string::npos)
    {
        return str;
    }
    string::size_type pos2 = str.find_last_not_of(tags);
    if(pos2 != string::npos)
    {
        return str.substr(pos, pos2 - pos + 1);
    }
    return str.substr(pos);
}

/*
 * generate a vector of random float
 * between 0 to 1
 */
void genRandVec(int lenVec, vector<ItemType> &vec)
{
    vec.reserve(lenVec);
    float rv;
    //srand((unsigned) time(NULL));
    for(int i=0; i<lenVec; i++)
    {
        //rv = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5) / 100;
        rv = 0.01;
        vec.push_back(rv);
    }
}

void normVec(vector<ItemType> &vec)
{
    typedef vector<ItemType>::iterator itype;
    float ran_sum = 0.0;
    for(itype it=vec.begin(); it!=vec.end(); ++it)
    {
        ran_sum += (*it * *it);
    }
    ran_sum = sqrt(ran_sum);

    for(itype it=vec.begin(); it!=vec.end(); ++it)
    {
        *it = *it / ran_sum;
    }
}

void genNormRandVec(int lenVec, vector<ItemType> &vec)
{
    genRandVec(lenVec, vec);
    //normVec(vec);
}

void showVec(vector<ItemType> &vec)
{
    for(vector<ItemType>::iterator it=vec.begin();
            it!=vec.end(); ++it)
    {
        cout << *it << " ";
    }
    cout << endl;
}

// add from_vector to to_vector
void addVec(const vector<ItemType> &from, vector<ItemType> &to)
{
    assert(from.size() == to.size());

    for(int i=0; i<from.size(); i++)
    {
        to[i] += from[i];
    }
}

// operator of vectors
class Vec {

public:

    typedef ItemType value_type;
    typedef unsigned int index_type;
    typedef vector<value_type>::iterator iterator;
    typedef vector<value_type>::const_iterator const_iterator;

    Vec() {}

    Vec(int size)
    {
        vec.reserve(size);
        vec.resize(size, 0.0);
    }

    Vec(const Vec &vec): vec(vec.vec) { }

    Vec(vector<value_type> &vec)
    {
        this->vec = vec;
    }

    value_type dot(Vec &other)
    {
        //assert(vec.size() == other.size());
        value_type sum = 0.0;

        for(index_type i=0; i<vec.size(); ++i)
        {
            sum += vec[i] * other[i];
        }
        return sum;
    }

    void append(value_type val)
    {
        vec.push_back(val);
    }

    void clear()
    {
        vec.clear();
    }

    void norm()
    {
        normVec(vec);
    }

    float mean() {
        double sum = 0.0;
        for(vector<value_type>::iterator it=vec.begin(); it!=vec.end(); ++it)
        {
            sum += *it;
        }
        return sum / size();
    }

    float sum() {
        float sum = 0.0;
        for(vector<value_type>::iterator it=vec.begin(); it!=vec.end(); ++it)
        {
            sum += *it;
        }
        return sum;
    }

    bool empty() {
        return vec.empty();
    }

    value_type operator[](index_type id) const
    {
        return vec[id];
    }

    value_type& operator[](index_type id)
    {
        return vec[id];
    }

    Vec& operator+=(const Vec& other)
    {
        assert(vec.size() == other.size());
        for(int i=0; i<vec.size(); ++i)
        {
            vec[i] += other[i];
        }
        return *this;
    }

    Vec& operator/=(float val)
    {
        assert(val != 0.0);
        for(int i=0; i<vec.size(); ++i)
        {
            vec[i] /= val;
        }
        return *this;
    }

    Vec& operator-=(float val)
    {
        for(int i=0; i<vec.size(); ++i)
        {
            vec[i] -= val;
        }
        return *this;
    }

    Vec& operator-=(const Vec& other)
    {
        assert(size() == other.size());
        for(int i=0; i<vec.size(); ++i)
        {
            vec[i] -= other[i];
        }
        return *this;
    }

    Vec& operator=(const Vec& other)
    {
        assert(vec.size() == other.size());
        if(this != &other)
        {
            vec.resize(other.size());
            for(int i=0; i<vec.size(); i++)
            {
                vec[i] = other[i];
            }
        }
        return *this;
    }

    friend Vec operator/(Vec &vec, float val)
    {
        assert( val != 0.0);
        Vec newVec(vec);
        for(vector<value_type>::iterator it=newVec.begin();
                it!=newVec.end(); ++it)
        {
            *it /= val;
        }
        return newVec;
    }

    friend Vec operator*(Vec &vec, float val)
    {
        Vec newVec(vec);
        for(vector<value_type>::iterator it=newVec.begin();
                it!=newVec.end(); ++it)
        {
            *it *= val;
        }
        return newVec;
    }

    index_type size()
    {
        return vec.size();
    }

    index_type size() const
    {
        return vec.size();
    }

    void show()
    {
        cout << "size: --------" << size() << "---------" << endl;
        showVec(vec);
    }
    
    // iterators
    iterator begin()
    {
        return vec.begin();
    }


    iterator end()
    {
        return vec.end();
    }

    vector<value_type>& getVector()
    {
        return vec;
    }



private:
    vector<value_type> vec;
}; // end class Vec



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

vector<vstr>  genWindowsFromSentence(costr sent, int size)
{
    vstr words; 
    split(sent, words, " ");
    vector< vstr > windows;
    // skip sentences too short
    if(words.size() < size) 
    {
        // cout << "skip sentence:" << sent << endl;
        return windows;
    }

    for (int i=0; i<words.size() - size; i++)
    {
        vector<string> window; 
        window.assign(words.begin()+i, words.begin()+i+size);
        windows.push_back(window);
    }
    return windows;
}






//
}; // end namespace


#endif
