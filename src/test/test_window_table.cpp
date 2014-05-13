#include "../windowtable.h"
#include "../vocab.h"
using namespace std;
using namespace sent2vec;

void test_gen_windows_from_sentence()
{
    test("genWindowsFromSentence");

    string sent = "one two three four five six seven eight nine ten";
    vector<vstr> windows = genWindowsFromSentence(sent, 2);

    for(int i=0; i<windows.size(); i++)
    {
        vstr win = windows[i];
        for(int j=0; j<win.size(); j++)
        {
            cout << "'" << win[j] << "'" << " ";
        }
        cout << endl;
    }
}

void test_windows2WordIndexPair()
{
    test("windows2WordIndexPair");
    string sent = "one two three four five six seven eight nine ten one two one two";
    Vocab vocab(50);
    vocab.addFromSentence(sent);
    cout << "vocab size; " << vocab.size() << endl;

    vector<vstr> windows = genWindowsFromSentence(sent, 2);
    vstr indexs = windows2WordIndexPair(vocab, windows);

    for(int i=0; i<indexs.size(); ++i)
    {
        cout << indexs[i] << " ";
    }
    cout << endl;
}

void test_initWinCountDicFromFile()
{
    test("initWinCountDicFromFile");

    Vocab vocab(50);
    vocab.initFromFile("1.txt");

    WindowTable wt(2, &vocab, "1.txt");
    WindowTable::dicType winCountDic = wt.initWinCountDicFromFile();
    for(WindowTable::dicType::iterator it=winCountDic.begin();
            it != winCountDic.end(); ++it)
    {
        cout << it->first << "->" << it->second << endl;
    }
}

void test_genTable()
{
    test("initWinCountDicFromFile");

    Vocab vocab(50);
    vocab.initFromFile("1.txt");

    WindowTable wt(2, &vocab, "1.txt", 100);
    const vector<IndexType> table = wt.genTable();
    for(int i=0; i<100; i++)
    {
        cout << table[i] << endl;
    }
}

void test_getSamples()
{
    test("getSamples");

    Vocab vocab(50);
    vocab.initFromFile("1.txt");

    WindowTable wt(2, &vocab, "1.txt", 100);
    vstr samples = wt.getSamples(5);
    cout << "get samples: " << endl;
    for(int i=0; i<samples.size(); ++i)
    {
        cout << "s: " << samples[i] << endl;
    }
}


int main()
{
    test_gen_windows_from_sentence();
    test_windows2WordIndexPair();
    test_initWinCountDicFromFile();
    test_genTable();
    test_getSamples();

    return 0;
}

