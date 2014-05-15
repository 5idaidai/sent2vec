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


class Predict(object):
    '''
    get sentence's vector
    and word's vector
    '''
    def __init__(self, path, len_vec):
        '''
        path: model path
        '''
        self.path = path
        self.len_vec = 50
        self._init_model()

    def _init_model(self):
        data = mod_from_file(self.path)
        self.vocab_dic = data['vocab_dic']
        self.sent_dic = data['sent_dic']
        self.vocab_vecs = Arr(data['vocab_vecs'], self.len_vec)
        self.sent_vecs = Arr(data['sent_vecs'], self.len_vec)

    def get_word_vec(self, word):
        id = self.vocab_dic[word]
        return self.vocab_vecs[id]

    def get_sent_vec(self, sentence):
        id = self.sent_dic[sentence]
        return self.sent_dic[id]

if __name__ == '__main__':
    p = Predict("models/2.pk", 50)
    print "ha", p.get_word_vec("ha")
