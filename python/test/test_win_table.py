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
from window_table import *
from vocab import Vocab

class TestWindowTable(unittest.TestCase):
    def setUp(self):
        self.sent = "Syphilis cases have doubled in the United States since 2005 the Centers for Disease Control and Prevention said Thursday yes go yes go"
        self.vocab = Vocab()
        self.vocab.add_from_sent( self.sent.split())
        

    def test_gen_windows_from_sentence(self):
        words = self.sent.split()
        length = len(words)
        size = 2
        wins = gen_windows_from_sentence(words, size)
        print 'wins', wins
        self.assertEqual(length - size + 1, len(wins))

    def test_windows_to_word_index_pair(self):
        words = self.sent.split()
        length = len(words)
        size = 2
        wins = gen_windows_from_sentence(words, size)

        pairs = windows_to_word_index_pair(self.vocab, wins)
        print pairs

    def test_window_word_index_to_count_dic(self):
        words = self.sent.split()
        length = len(words)
        size = 2
        wins = gen_windows_from_sentence(words, size)

        pairs = windows_to_word_index_pair(self.vocab, wins)
        dic = window_word_index_to_count_dic(pairs)
        print 'count dic', dic

    def test_window_table(self):
        self.window_table = WindowTable(self.vocab)
        self.window_table([ self.sent.split() ] )
        print self.window_table.table



if __name__ == "__main__":
    unittest.main()

