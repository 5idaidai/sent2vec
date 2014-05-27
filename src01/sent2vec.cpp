#include "sent2vec.h"

using namespace std;
using namespace sent2vec;


int main()
{
    assert(1 == 2);
    srand((unsigned) time(NULL));
    Sent2Vec sent2vec("1.sample", 
        3, 
        3, 
        20, 
        0.5, 
        0.01, 
        "models/1");

    sent2vec.initData();
    for (int i=0; i<100; i++) {
        cout << i << endl;
        sent2vec.run();
        if(sent2vec.isConverge()) {
            cout << "detect convergence: " << endl;
            //Js.show();
            break;
        }
    }

    //sent.tofile("2.sent");
    //vocab.tofile("2.vocab");
    sent2vec.tofile();
    printf("main done!");
    return 0;
}
