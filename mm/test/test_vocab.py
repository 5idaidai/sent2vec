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
from vocab import Vocab

class TestVocab(unittest.TestCase):
    def setUp(self):
        self.words = \
            "hello are you superjom".split()

        self.vocab = Vocab()

    def test_add_from_sent(self):
        self.vocab.add_from_sent(self.words)
        self.assertEqual(len(self.vocab.vocab),
            len(set(self.words)))

    def test_add(self):
        for no,w in enumerate(self.words):
            self.vocab.add(w)
            self.assertEqual(
                self.vocab.vocab[w],
                no)

    def test_init_vecs(self):
        self.vocab.add_from_sent(self.words)
        self.vocab.init_vecs()

        self.assertEqual(
            self.vocab.vecs.shape[0], len(self.words))


    def test_getitem(self):
        self.vocab.add_from_sent(self.words)
        self.vocab.init_vecs()
        vec = self.vocab[self.words[0]]
        self.assertEqual(
            vec.shape, (50,))

    def test_get_window_vec(self):
        words = "hello are you superjom".split()
        self.vocab.add_from_sent(words)
        vecs = self.vocab.get_window_vec(
            "hello".split())
        print 'vecs:', vecs





if __name__ == "__main__":
    unittest.main()

