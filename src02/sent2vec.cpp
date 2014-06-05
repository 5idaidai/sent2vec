#include "sent2vec.h"
#include "cmdline.h"
#include "time.h"

using namespace std;
using namespace sent2vec;


int main(int argc, char *argv[])
{
    //assert(1 == 2);
    CMDLine cmdline(argc, argv);
    costr param_trainset_ph = cmdline.registerParameter("train", "path of the training set");
    costr param_window_size = cmdline.registerParameter("ws", "size of window, default is 3");
    costr param_nthreads = cmdline.registerParameter("nthreads", "number of threads");
    costr param_k = cmdline.registerParameter("k", "number of negative samples, default is 15");
    costr param_alpha = cmdline.registerParameter("alpha", "learning rate, default is 0.1");
    costr param_convergence = cmdline.registerParameter("c", "convergence rate, default is 0.01");
    costr param_model_ph = cmdline.registerParameter("o", "model output path");
    costr param_help = cmdline.registerParameter("help", "this scree");

    // args
    string train_ph;
    int windowSize = 3;
    int nThreads = 1;
    int k = 15;
    float alpha = 0.1;
    float convergence = 0.01;
    string model_ph;

    if(cmdline.hasParameter(param_help) || argc == 1) {
        cout << "============================" << endl;
        cout << "sentence to vector" << endl;
        cout << "============================" << endl;
        cmdline.print_help();
        return 0;
    }
    if(! cmdline.hasParameter(param_trainset_ph)) {
        cout << "missing " << param_trainset_ph << " arg" << endl;
        return 0;
    }
    train_ph = cmdline.getValue(param_trainset_ph);

    if (cmdline.hasParameter(param_window_size)) {
        windowSize = stoi(cmdline.getValue(param_window_size));
    }

    if (cmdline.hasParameter(param_nthreads)) {
        nThreads = stoi(cmdline.getValue(param_nthreads));
    }

    if (cmdline.hasParameter(param_k)) {
        k = stoi(cmdline.getValue(param_k));
    }

    if (cmdline.hasParameter(param_alpha)) {
        alpha = stof(cmdline.getValue(param_alpha));
    }

    if (cmdline.hasParameter(param_convergence)) {
        convergence = stof(cmdline.getValue(param_convergence));
    }

    if (cmdline.hasParameter(param_model_ph)) {
        model_ph = cmdline.getValue(param_model_ph);
    }

    srand((unsigned) time(NULL));
    Sent2Vec sent2vec(
        train_ph,
        windowSize,  // windowsize
        nThreads, // n thread
        k, // k
        alpha, // alpha
        convergence, // convergence
        model_ph
        );
    time_t time_start = time(0);
    sent2vec.initData();
    sent2vec.run();

    //sent.tofile("2.sent");
    //vocab.tofile("2.vocab");
    sent2vec.tofile();
    time_t time_end = time(0);
    double time = difftime(time_end, time_start) * 1000.0;
    printf("main done!");
    cout << "spent time: " << time << endl;
    return 0;
}
