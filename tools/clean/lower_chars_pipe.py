#!/usr/bin/python
# -*- coding: utf-8 -*-
'''
Created on Feb 24, 2014

@author: Chunwei Yan @ PKU
@mail:  yanchunwei@outlook.com
'''
import sys

while True:
    sentence = sys.stdin.readline().strip()
    if not sentence:
        break

    words = [w.lower() for w in sentence.split()]
    sentence = ' '.join(words)

    if not sentence:
        sentence = "EMPTY"
    sys.stdout.write(sentence+"\n")
