#!/usr/bin/python
# -*- coding: utf-8 -*-
'''
Created on Feb 24, 2014

@author: Chunwei Yan @ PKU
@mail:  yanchunwei@outlook.com
'''

from nltk.stem.porter import *
stemmer = PorterStemmer()

import sys



for line in sys.stdin.readlines():
    output = []
    line = line.strip()
    ws = [stemmer.stem(w) for w in line.split()]
    output.append(' '.join(ws))

    sentence = '\n'.join(output)
    sys.stdout.write(sentence+"\n")
