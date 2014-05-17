#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
Created on March 7, 2014

@author: Chunwei Yan @ PKU
@mail:  yanchunwei@outlook.com
'''
from __future__ import division
import os
import logging
import numpy as np
import multiprocessing as mp
from utils import mod2file, mod_from_file
logging.basicConfig(filename = os.path.join(os.getcwd(), '1.log'), level = logging.INFO)

class Vocab(object):
    '''
    建立词库
    包含 word vector 的对应关系
    each word has following fields:
        index: pos in numpy vectors
    '''
    def __init__(self, len_vec=50, vocab={}, vecs=None):
        self.len_vec = len_vec
        # key is hash(str)
        self.vocab = vocab
        self.vecs = vecs
        self.cur_idx = 0
        logging.info("create vocab len_vec(%d)" % len_vec)

    def get_window_vec(self, words = [], word_index=""):
        if self.vecs is None:
            self.init_vecs()
        win_vec = np.zeros(self.len_vec)
        if words:
            for w in words:
                index = self.vocab[hash(w)]
                win_vec += self.vecs[index]
            return win_vec

        if word_index:
            ids = [int(i) for i in word_index.split('-')]
            for id in ids:
                win_vec += self.vecs[id, :]
            return win_vec

    def add(self, word):
        key = hash(word)
        if key not in self.vocab:
            self.vocab[key] = self.cur_idx
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

    def export_mp_data(self, manager=None):
        '''
        return:
            shared vocab dic, vecs
            and original dic and vecs
        '''
        if self.vecs is None:
            self.init_vecs()
        if manager is None:
            manager = mp.Manager()
        ori_vocab = self.vocab
        vocab = manager.dict(ori_vocab)
        ori_vecs = self.vecs.reshape( len(self.vocab) * self.len_vec)
        vecs = mp.Array('d', ori_vecs)
        return vocab, vecs, ori_vocab, ori_vecs

    def __getitem__(self, key):
        if self.vecs is None:
            self.init_vecs()
        key = hash(key)
        index = self.vocab[key]
        return self.vecs[index]

    def __str__(self):
        return "<Vocab: %d words>" % len(self.vocab)
