build:
	g++ --std=c++20 -O3 -g -DNDEBUG -o integer-lzend -Iips4o/include/ips4o -Irmq/include -Iordered/include libsais/libsais.c integer-lzend.cpp
