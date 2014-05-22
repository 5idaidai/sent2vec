CXXFLAGS=-O2
CXX=g++

.PHONY: all clean

all: sent2vec test_sent test_vec test_utils test_vocab test_window_table psent2vec

psent2vec: pthread_sent2vec.cpp  utils.h sent.h vocab.h windowtable.h ../threads/thread.cpp ../wqueue/wqueue.h
	g++ $(CXXFLAGS) pthread_sent2vec.cpp ../threads/thread.cpp -o psent2vec -lpthread

sent2vec: sent2vec.cpp  utils.h sent.h vocab.h windowtable.h
	g++ $(CXXFLAGS) sent2vec.cpp -o sent2vec -lpthread

test_sent: test/test_sent.o
	g++ test/test_sent.o -o test/test_sent -lpthread

test_vec: test/test_vec.o
	g++ test/test_vec.o -o test/test_vec -lpthread

test_utils: test/test_utils.o
	g++ test/test_utils.o -o test/test_utils -lpthread

test_window_table: utils.h vocab.h test/test_window_table.o
	g++ test/test_window_table.o -o test/test_window_table -lpthread

test_vocab: utils.h vocab.h test/test_vocab.o
	g++ test/test_vocab.o -o test/test_vocab -lpthread

test/test_sent.o: sent.h utils.h test/test_sent.cpp
	g++ -c test/test_sent.cpp -lpthread
	mv test_sent.o test/

test/test_vec.o: utils.h test/test_vec.cpp
	g++ -c test/test_vec.cpp -lpthread
	mv test_vec.o test/

test/test_vocab.o: utils.h sent.h vocab.h test/test_vocab.cpp
	g++ -c test/test_vocab.cpp -lpthread
	mv test_vocab.o test/

test/test_utils.o: utils.h test/test_utils.cpp
	g++ -c test/test_utils.cpp -lpthread
	mv test_utils.o test/

test/test_window_table.o: utils.h windowtable.h vocab.h test/test_window_table.cpp
	g++ -c test/test_window_table.cpp -lpthread
	mv test_window_table.o test/

sent2vec.o: sent2vec.cpp utils.h sent.h vocab.h windowtable.h
	g++ -c sent2vec.cpp -lpthread


clean:
	rm *.o
	rm test/*.o
