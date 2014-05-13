#include <iostream>
#include "../utils.h"
using namespace std;
using namespace sent2vec;

void test_trim()
{
    string line = "\t\r hello world\r\n ";
    cout << "ori: " << line << endl;
    line = trim(line);
    cout << "aft: " << line << endl;
}

void test_split()
{
    string line = "hello world yes go to that" ;

    vector<string> words;
    split(line, words, " ");
    for(int i=0; i<words.size(); i++)
    {
        cout << "w: '" << words[i] <<"'" << endl;
    }
}

void test_join()
{
    vstr words;
    words.push_back("hello");
    words.push_back("word");
    words.push_back("yes");
    words.push_back("go");
    words.push_back("!");

    cout << "'" << join(words, "-") << "'" << endl;
    cout << "'" << join(words, " ") << "'" << endl;
}

int main()
{
    test_trim();
    test_split();
    test_join();
}



