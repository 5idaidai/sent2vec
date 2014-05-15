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

// test operator/
void test_div()
{
    test("div");
    Vec vec(4);
    vec[0] = 2;
    vec[1] = 1;
    vec[2] = 0;
    vec[3] = 4;

    cout << "ori:" << endl;
    vec.show();
    cout << "after / 3" << endl;
    Vec a = vec / 3;
    a.show();

    cout << "after norm" << endl;
    a.norm();
    a.show();
}


int main()
{
    test_constrct_size();
    test_dot();
    test_norm();
    test_add_eq();
    test_eq();
    test_div();


    return 0;
}
