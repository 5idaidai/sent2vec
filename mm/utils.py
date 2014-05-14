#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
Created on March 7, 2014

@author: Chunwei Yan @ PKU
@mail:  yanchunwei@outlook.com
'''
import cPickle as pk

def mod2file(mod, path):
    with open(path, 'w') as f:

        pk.dump(mod, f)

def mod_from_file(path):
    with open(path) as f:
        return pk.load(f)


if __name__ == '__main__':
    a = range(10)
    mod2file(a, '1.tmp')
    b = mod_from_file('1.tmp')
    print 'b', b
