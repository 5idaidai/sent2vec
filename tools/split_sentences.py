#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
Created on March 7, 2014

@author: Chunwei Yan @ PKU
@mail:  yanchunwei@outlook.com
'''
import sys
import nltk.data
sent_detector = nltk.data.load('tokenizers/punkt/english.pickle')
text = sys.stdin.read()
sentences = sent_detector.tokenize(text.strip())

sys.stdout.write("\n".join(sentences))
