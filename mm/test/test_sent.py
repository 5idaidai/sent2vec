#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
Created on March 7, 2014

@author: Chunwei Yan @ PKU
@mail:  yanchunwei@outlook.com
'''
import sys
sys.path.append('..')
import unittest
import numpy as np
from sent import Sent
from utils import Arr

class TestSent(unittest.TestCase):
    def setUp(self):
        self.sents = [
            "hello world",
            "yes let go",
            "your go",
            "haha haha\n",
            ]

        self.sent = Sent()
        for s in self.sents:
            self.sent.add(s)


    def test_add(self):
        self.assertEqual(len(self.sent.sent), len(self.sents))
        for sent in self.sents:
            self.assertTrue( sent.strip() in self.sent.sent)

    def test_init_vecs(self):
        self.sent.init_vecs()
        self.assertEqual(
            self.sent.vecs.shape[0], len(self.sents))

    def test_getitem(self):
        for no,sent in enumerate(self.sents):
            key = sent.strip()
            self.assertEqual(
                no, self.sent.sent[key])

    def test_vecs(self):
        '''
        test the API of Arr
        '''
        import multiprocessing as mp
        manager = mp.Manager()
        print self.sent.sent
        self.sent.init_vecs()
        sent = manager.dict(self.sent.sent)
        vecs = self.sent.vecs.reshape( len(self.sent.sent) * 50)
        vecs = mp.Array('d', vecs)
        vecs = np.frombuffer(vecs.get_obj())
        print vecs

        new_sent = Sent(50, sent, vecs)
        print new_sent["hello world"]






        




if __name__ == "__main__":
    unittest.main()
    

