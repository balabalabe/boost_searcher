FLAG=-I ~/third_part/include -std=c++11 -ljsoncpp

.PHONY:all
all:index_test searcher_test libsearcher.so

index_test:index_test.cc searcher.cc
	g++ $^ -o $@ $(FLAG)

searcher_test:searcher_test.cc searcher.cc
	g++ $^ -o $@ $(FLAG)

libsearcher.so:searcher.cc searcher.h
	g++ searcher.cc -o libsearcher.so -shared -fPIC $(FLAG)

.PHONY:clean
clean:
	rm index_test searcher_test libsearcher.so

