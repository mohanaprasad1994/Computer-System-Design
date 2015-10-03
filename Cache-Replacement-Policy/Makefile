# Makefile
#
SH = bash
CFLAGS = -g
LDFLAGS = -lm

all: main

main: cache.o tree_pseudo_lru.o main.o
	g++ $(LDFLAGS) $^ -o $@

main.o: main.cpp Cache.h TreePseudoLru.h
	g++ -c $(CFLAGS) $< -o $@

cache.o: Cache.cpp Cache.h CacheReplacementPolicy.h
	g++ -c $(CFLAGS) $< -o $@


tree_pseudo_lru.o: TreePseudoLru.cc TreePseudoLru.h
	g++ -c $(CFLAGS) $< -o $@

clean:
	rm -rf *.o cache

