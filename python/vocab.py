#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
Created on March 7, 2014

@author: Chunwei Yan @ PKU
@mail:  yanchunwei@outlook.com
'''
from __future__ import division
import numpy as np
from utils import mod2file, mod_from_file

class Vocab(object):
    '''
    建立词库
    包含 word vector 的对应关系
    each word has following fields:
        index: pos in numpy vectors
    '''
    def __init__(self, len_vec=50):
        self.len_vec = len_vec
        self.vocab = {}
        self.vecs = None
        self.cur_idx = 0

    def get_window_vec(self, words = [], word_index=""):
        if self.vecs is None:
            self.init_vecs()
        win_vec = np.zeros(self.len_vec)
        if words:
            for w in words:
                index = self.vocab[w]
                win_vec += self.vecs[index]
            return win_vec

        if word_index:
            ids = word_index.split('-')
            for id in ids:
                win_vec += self.vecs[id, :]
            return win_vec

    def add(self, word):
        if word not in self.vocab:
            self.vocab[word] = self.cur_idx
            self.cur_idx += 1

    def add_from_sent(self, sent):
        '''
        sent: list of words
        '''
        if not type(sent) == type([]):
            sent = sent.split()
        for w in sent:
            self.add(w)

    def init_vecs(self):
        print 'create vocabulary %d' % len(self.vocab)
        length = len(self.vocab)
        self.vecs = np.random.randn(length, self.len_vec).astype('float32')

    def tofile(self, path):
        print 'save Vocab to (%s)' % path
        mod2file(self.vocab, path)

    def fromfile(self, path):
        print 'load Vocab from (%s)' % path
        self.vocab = mod_from_file(path)

    def __getitem__(self, key):
        if self.vecs is None:
            self.init_vecs()
        index = self.vocab[key]
        return self.vecs[index]


    def __str__(self):
        return "<Vocab: %d words>" % len(self.vocab)
