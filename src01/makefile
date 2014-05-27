CXXFLAGS=-O1
CXX=g++

.PHONY: all clean

all: sent2vec

sent2vec: sent2vec.cpp sent2vec.h utils.h sent.h vocab.h table.h ../threads/thread.cpp ../wqueue/wqueue.h
	g++ $(CXXFLAGS) sent2vec.cpp ../threads/thread.cpp -o sent2vec -lpthread 
