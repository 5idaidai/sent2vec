#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
Created on March 7, 2014

@author: Chunwei Yan @ PKU
@mail:  yanchunwei@outlook.com
'''
from __future__ import division
import math
import numpy as np
from random import random


def gen_windows_from_sentence(sent, size):
    '''
    args:
        @sent: list of words
        @size: size of window
    '''
    if type(sent) == type(""):
        sent = sent.split()

    windows = []
    for i in range(len(sent) - size + 1):
        windows.append( sent[i: i+size])
    return windows

def windows_to_word_index_pair(vocab, windows):
    indexs = []
    for w in windows:
        idx = "-".join([str(vocab.vocab[v]) for v in w])
        indexs.append(idx)
    return indexs

def window_word_index_to_count_dic(windows):
    '''
    windows:
        list of index pair
    '''
    dic = {}
    for w in windows:
        if w not in dic:
            dic[w] = 0
        dic[w] += 1
    return dic



class WindowTable(object):
    '''
    a table for NCE
    '''
    TABLE_SIZE = int(1e7)
    POWER = 0.75

    def __init__(self, vocab, size=3):
        '''
        self.window:
            dic of windows, fields: count, idx in window table
        '''
        self.size = size
        self.vocab = vocab
        self.win_counts = []

    def __call__(self, sents):
        '''
        sents:
            list of word 
        '''
        win_count_dic = self._gen_win_count_dic_from_sent(sents)
        self.gen_table(win_count_dic)


    def _gen_win_count_dic_from_sent(self, sents):
        win_count_dic = {}
        for sent in sents:
            if type(sent) != type([]):
                sent = sent.split()
            windows = gen_windows_from_sentence(sent, self.size)
            indexs = windows_to_word_index_pair(self.vocab, windows)
            for idx in indexs:
                if not idx in win_count_dic:
                    win_count_dic[idx] = 0
                win_count_dic[idx] += 1
        return win_count_dic

    def gen_table(self, win_count_dic={}):
        self.table = np.empty( WindowTable.TABLE_SIZE, np.int64)
        # gen (window, count) pair
        vocab = win_count_dic.items()
        self.win_counts = vocab
        vocab_size = len(vocab)

        train_windows_pow_sum = sum(
            math.pow(v[1], WindowTable.POWER) for v in vocab)
        print 'get train_windows_pow_sum', train_windows_pow_sum

        i = 0
        d1 = math.pow(vocab[i][1]/train_windows_pow_sum, 
                        WindowTable.POWER)
        for a in xrange(self.TABLE_SIZE):
            self.table[a] = i
            if a*1.0 / WindowTable.TABLE_SIZE > d1:
                i += 1
                d1 += math.pow(vocab[i][1], WindowTable.POWER) / train_windows_pow_sum
            if i >= vocab_size:
                i = vocab_size - 1

    def get_samples(self, k):
        '''
        Noise-constrastive estimation
        return list of (index, key)s
        index is the pos of the sample in window table
        key is the key of the window
        '''
        samples = []
        for i  in xrange(k):
            rand = int(random() * WindowTable.TABLE_SIZE)
            idx = self.table[rand]
            key = self.win_counts[idx][0]
            samples.append(
                (idx, key,))
        return samples

    def __getitem__(self, index):
        return self.table[index]



if __name__ == "__main__":
    pass

