#!/usr/bin/python
# -*- coding: utf-8 -*-
'''
Created on March 10, 2014

@author: Chunwei Yan @ PKU
@mail:  yanchunwei@outlook.com
'''
import re
import sys
from nltk.tokenize import RegexpTokenizer

def rm_punc(sentence):
    token = RegexpTokenizer(r'\w+')
    sentence = ' '.join(token.tokenize(sentence))
    return sentence

if __name__ == '__main__':
    for sentence in sys.stdin.readlines():
        sentence = sentence.strip()

        sentence = rm_punc(sentence)

        #sentence = re.sub("(('')|[_].|\(.*\))|([.]$)", "", sentence)
        
        #words = [w.lower() for w in sentence.split()]
        #sentence = ' '.join(words)
        if not sentence:
            sentence = "EMPTY"

        sys.stdout.write(sentence+"\n")
