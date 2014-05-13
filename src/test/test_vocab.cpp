#include "../vocab.h"
using namespace std;
using namespace sent2vec;

void test_initFromFile()
{
    test("initFromFile");
    Vocab vocab(50);
    vocab.initFromFile("1.txt");
    cout << "size: " << vocab.size() << endl;
}

int main()
{
    test_initFromFile();

    return 0;
}

