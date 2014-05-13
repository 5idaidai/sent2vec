#include <iostream>
#include "../utils.h"
using namespace std;
using namespace sent2vec;

void test_constrct_size()
{
    test("construct_size");
    Vec vec(100);
    cout << "vec.size " << vec.size() << endl;
    cout << "vec[10]" << vec[10] << endl;
}

void test_dot()
{
    test("dot");
    Vec vec(3);
    vec[0] = 2;
    vec[1] = 1;
    vec[2] = 0;
    vec.show();

    Vec vec2(3);
    vec2[0] = 1;
    vec2[1] = 3;
    vec2[2] = 9;
    vec2.show();

    float res = vec.dot(vec2);
    cout << "dot : "<< res << endl;
}

void test_norm()
{
    test("norm");
    Vec vec(3);
    vec[0] = 2;
    vec[1] = 1;
    vec[2] = 0;
    vec.show();
    vec.norm();
    vec.show();
}

void test_add_eq()
{
    test("+=");
    Vec vec(3);
    vec[0] = 2;
    vec[1] = 1;
    vec[2] = 0;
    vec.show();

    Vec vec2(3);
    vec2[0] = 1;
    vec2[1] = 3;
    vec2[2] = 9;
    vec2.show();

    vec += vec2;
    cout << "after +=" << endl;
    vec.show();
}

void test_eq()
{
    test("=");
    Vec vec(3);
    vec[0] = 2;
    vec[1] = 1;
    vec[2] = 0;
    vec.show();

    Vec vec2(3);
    vec2[0] = 1;
    vec2[1] = 3;
    vec2[2] = 9;
    vec2.show();

    vec = vec2;
    cout << "after =" << endl;
    vec.show();
}


int main()
{
    test_constrct_size();
    test_dot();
    test_norm();
    test_add_eq();
    test_eq();


    return 0;
}
