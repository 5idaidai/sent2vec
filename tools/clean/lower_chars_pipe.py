#!/usr/bin/python
# -*- coding: utf-8 -*-
'''
Created on Feb 24, 2014

@author: Chunwei Yan @ PKU
@mail:  yanchunwei@outlook.com
'''
import sys

def tolower(sent):
    words = [w.lower() for w in sent.split()]
    sentence = ' '.join(words)
    return sentence


if __name__ == '__main__':

    for sentence in sys.stdin.readlines():
        sentence = sentence.strip()

        sentence = tolower(sentence)

        if not sentence:
            sentence = "EMPTY"
        sys.stdout.write(sentence+"\n")
