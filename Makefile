build:
	g++ --std=c++20 -O3 -g -DNDEBUG -o integer-lzend -Irmq/include -Iordered/include libsais.c integer-lzend.cpp
