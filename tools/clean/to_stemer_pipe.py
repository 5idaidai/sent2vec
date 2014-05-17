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

def to_stemer(sentence):
    ws = [stemmer.stem(w) for w in sentence.split()]
    return ' '.join(ws)


if __name__ == '__main__':
    for line in sys.stdin.readlines():
        output = []
        line = line.strip()
        output.append(to_stemer(line))

        sentence = '\n'.join(output)
        sys.stdout.write(sentence+"\n")
