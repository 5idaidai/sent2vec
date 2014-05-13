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
from sent import Sent

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




        




if __name__ == "__main__":
    unittest.main()
    

