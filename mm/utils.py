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

class Arr(object):
    def __init__(self, arr, vec_size):
        # arr: numpy array
        self.arr = arr
        self.vec_size = vec_size

    def __getitem__(self, index, *args):
        if isinstance(index, tuple):
            index = index[0]
        start = index * self.vec_size
        end = start + self.vec_size
        return self.arr[start: end]


if __name__ == '__main__':
    import numpy as np
    a = Arr(np.array(range(900)), 30)
    print a[0, :]
