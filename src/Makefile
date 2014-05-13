CXX=g++
CXXFLAGS=-g

.PHONY: all

all: test_sent test_vec test_utils test_vocab test_window_table 

test_sent: test/test_sent.o
	g++ test/test_sent.o -o test/test_sent 

test_vec: test/test_vec.o
	g++ test/test_vec.o -o test/test_vec 

test_utils: test/test_utils.o
	g++ test/test_utils.o -o test/test_utils 

test_window_table: utils.h vocab.h test/test_window_table.o
	g++ test/test_window_table.o -o test/test_window_table 

test_vocab: utils.h vocab.h test/test_vocab.o
	g++ test/test_vocab.o -o test/test_vocab

test/test_sent.o: sent.h utils.h test/test_sent.cpp
	g++ -c test/test_sent.cpp
	mv test_sent.o test/

test/test_vec.o: utils.h test/test_vec.cpp
	g++ -c test/test_vec.cpp
	mv test_vec.o test/

test/test_vocab.o: utils.h sent.h vocab.h test/test_vocab.cpp
	g++ -c test/test_vocab.cpp
	mv test_vocab.o test/

test/test_utils.o: utils.h test/test_utils.cpp
	g++ -c test/test_utils.cpp
	mv test_utils.o test/

test/test_window_table.o: utils.h windowtable.h vocab.h test/test_window_table.cpp
	g++ -c test/test_window_table.cpp
	mv test_window_table.o test/



