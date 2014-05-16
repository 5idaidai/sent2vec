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
from sent2vec import Sent2Vec


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


def train_worker(vec_size, window_size, k,  alpha, queue, results_queue, sent_dic, sent_vecs, vocab_dic, vocab_vecs, table, win_count_dic, lock ):
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
        windows = gen_windows_from_sentence(sentence, window_size)
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


def producer(dataset, n_workers, queue, results_queue, n_turns=30):
    '''
    put  sentences to queue
    '''
    results = []
    def show_results_status(results):
        if results_queue.qsize() > 1:
            res = results_queue.get()
            if res is None:
                print 'J', np.mean(results)
                results = []
            else:
                results.append(res)

    for i in xrange(n_turns):
        for no, sent in enumerate(dataset.sents):
            show_results_status(results)
            queue.put(sent)
        results_queue.put(None)
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


def multi_process_run(data_path, model_output_path, k=2, n_workers=2, n_turns=10, queue_size=200, vec_size=50, window_size=2, alpha=0.1):
    s2v = Sent2Vec(data_path, vec_size=vec_size, k=k, alpha=alpha)
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
        args=(s2v.dataset, s2v.n_workers, queue, results_queue, n_turns ))
    p.start()
    ps.append(p)

    for i in xrange(n_workers):
        print 'start p', i
        p = mp.Process(target=train_worker, args=(vec_size, window_size, k, alpha, queue, results_queue, sent_dic, sent_vecs, vocab_dic, vocab_vecs, table, win_count_dic, lock, ))
        p.start()
        ps.append(p)

    for p in ps:
        p.join()
    
    dump_out_data(model_output_path, ori_vocab_dic, ori_vocab_vecs, ori_sent_dic, ori_sent_vecs, ori_table, ori_win_count_dic)


if __name__ == '__main__':
    multi_process_run("data/2.sample", 
                "models/2.pk", 
                k=2, 
                n_workers=2, 
                n_turns=20,
                window_size=3,
                )
