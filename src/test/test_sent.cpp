#include "../sent.h"
#define DEBUG
using namespace std;
using namespace sent2vec;

void test_add()
{
    cout << "test add " <<endl;

    Sent sent(50);
    sent.add("hello world\n");
    sent.add(" good world\n");
    cout << "hello world" << endl;

    bool res = sent.contains("hello world\r");
    cout << "contains hello world?" << res << endl;

    res = sent.contains("hello \r");
    cout << "contains hello ?" << res << endl;
}

void test_initVecs()
{
    cout << "test initVecs" << endl;

    Sent sent(50);
    sent.add("hello world\n");
    sent.add(" good world\n");
    sent.add(" gg world\n");
    sent.add(" hello world\n");
    sent.initVecs("../rand.txt");

}

void test_toVec()
{
    cout << "test initVecs" << endl;

    Sent sent(50);
    sent.add("hello world\n");
    sent.add(" good world\n");
    sent.add(" gg world\n");
    sent.add(" hello world\n");
    sent.initVecs("../rand.txt");
    Vec vec = sent.toVec("gg world");
    vec.show();
    // check sum of x^2
    float sum = 0.0;
    for(Sent::vec_iter it=vec.begin();
            it!=vec.end(); ++it)
    {
        sum += *it * *it; 
    }
    cout << "sum is " << sum << endl;
}

void test_operator()
{
    cout << "test initVecs" << endl;

    Sent sent(50);
    sent.add("hello world\n");
    sent.add(" good world\n");
    sent.add(" gg world\n");
    sent.add(" hello world\n");
    Sent::vec_type vec = sent["gg world"];
    vec.show();
    // check sum of x^2
    float sum = 0.0;
    for(Sent::vec_iter it=vec.begin();
            it!=vec.end(); ++it)
    {
        sum += *it * *it; 
    }
    cout << "sum is " << sum << endl;
}

void test_initFromFile()
{
    test("initFromFile");
    Sent sent(50);
    sent.initFromFile("../1.sample");
    cout << "size: " << sent.size() << endl;
}

void test_tofile()
{
    test("tofile");
    Sent sent(50);
    sent.initFromFile("../1.sample");
    sent.initVecs("../rand.txt");
    cout << "size: " << sent.size() << endl;
    sent.tofile("1.sent");
}

int main()
{
    test_initVecs();
    test_toVec();
    test_initFromFile();
    test_tofile();






    return 0;
}
