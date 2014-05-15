#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
Created on March 7, 2014

@author: Chunwei Yan @ PKU
@mail:  yanchunwei@outlook.com
'''
import numpy as np
import multiprocessing as mp
from utils import mod2file, mod_from_file

class Sent(object):
    '''
    '''
    def __init__(self, len_vec=50, sent={}, vecs=None):
        self.sent = sent
        self.cur_idx = 0
        self.vecs = vecs
        self.len_vec = len_vec

    def index(self, sent):
        sent = sent.strip()
        return self.sent[sent]

    def add(self, sent):
        '''
        sent: string
        '''
        sent = sent.strip()
        if sent not in self.sent:
            self.sent[sent] = self.cur_idx
            self.cur_idx += 1

    def init_vecs(self):
        print 'create sent map: %d' % len(self.sent)
        length = len(self.sent)
        self.vecs = np.random.randn(length, self.len_vec).astype('float32')


    def tofile(self, path):
        print 'save Sentences to (%s)' % path
        mod2file(self.sent, path)

    def fromfile(self, path):
        print 'load Vocab from (%s)' % path
        self.sent = mod_from_file(path)

    def export_mp_data(self, manager=None):
        if manager is None:
            manager = mp.Manager()
        sent = manager.dict(self.sent)
        vecs = self.vecs.reshape( len(self.sent) * self.len_vec)
        vecs = mp.Array('d', vecs)
        return [sent, vecs]

    def __str__(self):
        return "<Sent: %d sentences>" % len(self.vocab)

    def __getitem__(self, key):
        if self.vecs is None:
            self.init_vecs()
        key = key.strip()
        index = self.sent[key]
        return self.vecs[index]





