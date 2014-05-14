#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
Created on March 7, 2014

@author: Chunwei Yan @ PKU
@mail:  yanchunwei@outlook.com
'''
import sys
from nltk.tokenize import RegexpTokenizer

args = sys.argv[1:]


if not (args):
    print "./cmd.py file tofile"
    sys.exit(-1)

infile, tofile = args

with open(infile) as f:
    lines = []
    while True:
        line = f.readline()
        if not line: break
        line = line.strip()
        tokenizer = RegexpTokenizer(r'\w+')
        words = tokenizer.tokenize(line)
        line = " ".join([w.lower() for w in words if len(w) > 1])
        lines.append(line)

    with open(tofile, 'w') as g:
        g.write( '\n'.join(lines))
