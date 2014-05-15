#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
Created on March 7, 2014

@author: Chunwei Yan @ PKU
@mail:  yanchunwei@outlook.com
'''
from __future__ import division
import sys
import time
import math
import numpy as np
import threading
import multiprocessing as mp
from Queue import Queue
from numpy import linalg as LA
from vocab import Vocab
from sent import Sent
from utils import mod2file, mod_from_file, Arr
from window_table import WindowTable, gen_windows_from_sentence, windows_to_word_index_pair


SIZE = 2

VECTOR_SIZE = 50

class Dataset(object):
    '''
    dataset of sentences
    '''
    def __init__(self, path):
        self.path = path
        self._get_num_sents()

    def _get_num_sents(self):
        num = 0
        with open(self.path) as f:
            while True:
                line = f.readline()
                if not line:
                    break
                num += 0
        return num

    @property
    def sents(self):
        with open(self.path) as f:
            while True:
                line = f.readline()
                if not line:
                    break
                line = line.strip()
                yield line



class Sent2Vec(object):
    def __init__(self, path="", vec_size=50, k=20, alpha=0.1, n_workers=1):
        '''
        :parameters:

            @path: string
                path to dataset, should be a single file

            @vec_size: int
                size of sentence vector and word vector

            @k: int 
                number of negative samples for a window

            @alpha: float
                learning rate
        '''
        self.k = k
        self.vec_size = vec_size
        self.n_workers = n_workers
        self.alpha = alpha
        self.vocab = Vocab()
        self.sent = Sent()
        self.window_table = WindowTable(self.vocab, SIZE)
        self.dataset = Dataset(path)

        if path:
            self.create_vocab()
            self.create_sent()
            self.create_window_table()

    def create_vocab(self):
        for sent in self.dataset.sents:
            sent = sent.split()
            self.vocab.add_from_sent(sent)
        self.vocab.init_vecs()

    def create_sent(self):
        for sent in self.dataset.sents:
            self.sent.add(sent)
        self.sent.init_vecs()

    def create_window_table(self):
        '''
        for negative sampling
        '''
        self.window_table(self.dataset.sents)

    
    def multi_thread_train(self):
        '''
        use mini-batch to train
        '''
        jobs = Queue(maxsize=9 * self.n_workers)
        lock = threading.Lock()

        start, next_report = time.time(), [1.0]

        self.Js = []

        def worker_train():
            while True:
                # get sentence
                sent = jobs.get()
                if sent is None:
                    break

                Jn = self.train_sent(sent, lock)
                self.Js.append(Jn)

        workers = [threading.Thread(target=worker_train) for _ in xrange(self.n_workers)]

        for thread in workers:
            thread.daemon = True  # make interrupting the process with ctrl+c easier
            thread.start()
        # put dataset to Queue
        for sent in self.dataset.sents:
            jobs.put(sent)
        # put None to tell all threads to exit
        for _ in xrange(self.n_workers):
            jobs.put(None)  

        for thread in workers:
            thread.join()
        print 'Js: ', np.mean(self.Js)
        elapsed = time.time() - start
        print 'used time', elapsed

    def train(self):
        '''
        use mini-batch to train
        '''
        Js = []
        for no, sent in enumerate(self.dataset.sents):
            Jn = self.train_sent(sent)
            Js.append(Jn)
        # calculate Jn for this sentence
        mean_Js = np.mean( np.array(Js))
        print 'total J', mean_Js
        return mean_Js

    def train_sent(self, sent, lock=None):
        # the loss
        Jn = 0
        #print no, 
        #print 'training sent: ', no, sent
        # get windows from the sent
        windows = gen_windows_from_sentence(sent, SIZE)
        #print 'gen windows', windows
        # get sentence vector
        v = self.sent[sent]
        
        for wn, window in enumerate(windows):
            #print '.', 
            #assert( type(window) == type([]), "window is %s" % str(window))
            #print 'window', window
            window_key = "-".join([str(self.vocab.vocab[w]) for w in window])
            h = self.vocab.get_window_vec(word_index=window_key)
            # noises
            noises = self.window_table.get_samples(self.k)
            #n_hs = [self.vocab.get_window_vec(s[1]) for s in noises ]
            # for a positive sample
            #print 'h:', h
            #print 'v:', v
            e_vT_h = np.e**np.dot(v.T, h)
            #print "dot(v,h)", np.dot(v, h)
            #print "e_vT_h", e_vT_h
            #sys.exit(0);
            update_v = h / (1 + e_vT_h)
            update_h = v / (1 + e_vT_h)
            # add positive window's loss
            Jn += math.log( 1 / ( 1 + 1/e_vT_h))

            self.update_window(window_key, update_h, lock)
            # for each negative window sample
            for idx, key in noises:
                n_h = self.vocab.get_window_vec(word_index=key)
                e_vT_h = np.e ** np.dot(v, n_h)
                frac_e_v_h = 1 - \
                        1 / (1 + e_vT_h)
                # accumulate the gradient
                update_v += - n_h * frac_e_v_h
                update_n_h = - v * frac_e_v_h
                self.update_window(key, update_n_h, lock)
                # add noise's loss
                Jn += math.log( 1/ (1+e_vT_h))

            update_v /= ( 1 + self.k)
            # update sentence vector for each window
            # TODO change to a single turn?
            self.update_sent_vec(v, update_v, lock)
            # add loss to total Jn
        #print 
        return Jn

    def update_sent_vec(self, sent_vec, grad, lock=None):
        if lock:
            with lock:
                sent_vec += self.alpha * grad
                sent_vec /= LA.norm(sent_vec)
        else:
            sent_vec += self.alpha * grad
            sent_vec /= LA.norm(sent_vec)

    def update_window(self, key, grad, lock=None):
        '''
        update each word's vector in a window
            and norm the vectors

        :parameters:
            @key: string
                like '19-32-2'
            @grad: numpy.array
                the gradient
        '''
        word_ids = [int(id) for id in key.split('-')]
        for id in word_ids:
            word_vec = self.vocab.vecs[id]
            if lock:
                with lock:
                    word_vec += self.alpha * grad
                    word_vec /= LA.norm(word_vec)
            else:
                word_vec += self.alpha * grad
                word_vec /= LA.norm(word_vec)


    def tofile(self, path):
        '''
        save model to file
        '''
        mod2file(self, path)

    @staticmethod
    def fromfile(path):
        return mod_from_file(path)




##########################################################
def update_sent_vec(sent_vec, grad, lock, alpha):
    with lock:
        sent_vec += alpha * grad
        sent_vec /= LA.norm(sent_vec)


def update_window( vocab, key, grad, lock, alpha):
    word_ids = [int(id) for id in key.split('-')]
    for id in word_ids:
        word_vec = vocab.vecs[id]
        with lock:
            word_vec += alpha * grad
            word_vec /= LA.norm(word_vec)


def show_status(results_queue):
    qsize = results_queue.qsize()
    if qsize % 100 == 0:
        print '.. qsize:', qsize


def train_worker(vec_size, k, alpha, queue, results_queue, sent_dic, sent_vecs, vocab_dic, vocab_vecs, table, win_count_dic, lock ):
    # change shared Array to numpy array
    sent_vecs = Arr(np.frombuffer(sent_vecs.get_obj()), vec_size)
    vocab_vecs = Arr(np.frombuffer(vocab_vecs.get_obj()), vec_size)
    # init objects
    sent = Sent(vec_size, sent_dic, sent_vecs)
    vocab = Vocab(vec_size, vocab_dic, vocab_vecs)
    window_table = WindowTable(vocab, vec_size, table, win_count_dic)
    # get a task
    sentence = queue.get()
    while sentence != None:
        Jn = 0
        windows = gen_windows_from_sentence(sentence, SIZE)
        v = sent[sentence]
        for wn, window in enumerate(windows):
            window_key = "-".join([str(vocab.vocab[w]) for w in window])
            h = vocab.get_window_vec(word_index=window_key)
            # noises
            noises = window_table.get_samples(k)
            e_vT_h = np.e**np.dot(v.T, h)
            update_v = h / (1. + e_vT_h)
            update_h = v / (1. + e_vT_h)
            # add positive window's loss
            Jn += math.log( 1. / ( 1. + 1./e_vT_h))
            update_window(vocab, window_key, update_h, lock, alpha)
            for idx, key in noises:
                n_h = vocab.get_window_vec(word_index=key)
                e_vT_h = np.e ** np.dot(v, n_h)
                frac_e_v_h = 1 - \
                        1 / (1 + e_vT_h)
                # accumulate the gradient
                update_v += - n_h * frac_e_v_h
                update_n_h = - v * frac_e_v_h
                update_window(vocab, key, update_n_h, lock, alpha)
                # add noise's loss
                Jn += math.log( 1/ (1+e_vT_h))

            update_v /= ( 1 + k)
            update_sent_vec(v, update_v, lock, alpha)
        #return Jn
        results_queue.put(Jn)
        current = mp.current_process()
        #print "%s Jn: %f" % (current.name, Jn)
        sentence = queue.get()
        show_status(results_queue)
    print "process %s exit!" % current.name


def producer(dataset, n_workers, queue, n_turns=30):
    '''
    put  sentences to queue
    '''
    for i in xrange(n_turns):
        for no, sent in enumerate(dataset.sents):
            queue.put(sent)
        # prepare to exit
    for i in xrange(n_workers * 3):
        queue.put(None)
    print "producer exit!"


def dump_out_data(path, vocab_dic, vocab_vecs, sent_dic, sent_vecs, table, win_count_dic):
    '''
    Export model to a binary file
    '''
    data = {
        'vocab_dic': vocab_dic,
        'vocab_vecs': vocab_vecs,
        'sent_dic': sent_dic,
        'sent_vecs': sent_vecs,
        'table': table,
        'win_count_dic': win_count_dic,
        }
    mod2file(data, path)


def multi_process_run(data_path, model_output_path, k=2, n_workers=2, n_turns=10, queue_size=200):
    s2v = Sent2Vec(data_path, k=k)
    lock = mp.Lock()
    manager = mp.Manager()
    vocab_dic, vocab_vecs, ori_vocab_dic, ori_vocab_vecs = s2v.vocab.export_mp_data(manager)
    sent_dic, sent_vecs, ori_sent_dic, ori_sent_vecs = s2v.sent.export_mp_data(manager)
    table, win_count_dic, ori_table, ori_win_count_dic = s2v.window_table.export_mp_data(manager) 
    queue = manager.Queue(queue_size)
    results_queue = manager.Queue()
    print '.. start producer'

    ps = []
    p = mp.Process(
        target = producer,
        args=(s2v.dataset, s2v.n_workers, queue,n_turns ))
    p.start()
    ps.append(p)

    for i in xrange(n_workers):
        print 'start p', i
        p = mp.Process(target=train_worker, args=(s2v.vec_size, s2v.k, s2v.alpha, queue, results_queue, sent_dic, sent_vecs, vocab_dic, vocab_vecs, table, win_count_dic, lock, ))
        p.start()
        ps.append(p)

    for p in ps:
        p.join()
    
    # print result
    qsize = results_queue.qsize()
    print 'qsize', qsize
    results = [results_queue.get() for i in xrange(qsize)]
    len_scope = int(qsize / n_turns)

    for i in xrange(n_turns):
        start = i * len_scope
        J = np.mean(results[start:start+len_scope])
        print "result :", J

    dump_out_data(model_output_path, ori_vocab_dic, ori_vocab_vecs, ori_sent_dic, ori_sent_vecs, ori_table, ori_win_count_dic)


if __name__ == '__main__':
    multi_process_run("data/2.sample", "models/2.pk")
