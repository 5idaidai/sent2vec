#!/bin/bash

awk -F"\t" '{
    print $2;
}' | ./clean/rm_punctuations_pipe.py | ./clean/to_stemer_pipe.py | ./clean/lower_chars_pipe.py
