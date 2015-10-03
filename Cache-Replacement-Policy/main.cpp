#include <stdio.h>
#include <iostream>
#include <map>
#include <stdlib.h>

#include "Cache.h"
#include "TreePseudoLru.h"

using namespace std;
map <long long unsigned int , int> blocks_accessed;
long long unsigned int cacheline_size, cache_size,associativity;
long long unsigned int Mask_for_block = 0;
long long unsigned int num_blocks_accessed = 0;

int main(int argc, char** argv){
	

	cacheline_size = 32;
	cache_size = 1024;
	associativity = 8;

	for(int i=1;i<argc;i++){
      if(argv[i][0]=='-'){
	switch(argv[i][1]){
	  case 'b':cacheline_size=atoi(argv[i+1]);i++;break;
	  case 's':cache_size=atoi(argv[i+1]);i++;break;
	  case 'a':associativity=atoi(argv[i+1]);i++;break;
	  	  
	}
      }
    }
	
	Cache* cache = new Cache();
	cache->set_associativity (associativity);
	cache->set_line_size (cacheline_size);
	cache->set_size (cache_size);
	int num_sets = cache_size / (associativity * cacheline_size);
	CacheReplacementPolicy* cache_repl_policy = new TreePseudoLru(num_sets, associativity);
	cache->set_repl_policy (cache_repl_policy);
	cache->build_cache();

	Mask_for_block = ~(cacheline_size-1);
	long long unsigned int ins_addr, data_addr;
	char c;
	long long int num_misses = 0;
	long long int total_accesses = 0;
	while(scanf("%llx: %c %llx",&ins_addr,&c,&data_addr) == 3){
		// call func with ins_addr
		total_accesses += 2;
		if (!cache->cache_lookup(ins_addr))
			num_misses++;
		// call funct with data_addr
		if (!cache->cache_lookup(data_addr))
			num_misses++;
		blocks_accessed[(Mask_for_block&ins_addr)] = 1;
		blocks_accessed[(Mask_for_block&data_addr)] = 1;

	}

	num_blocks_accessed = blocks_accessed.size();
    cout<<"Total Misses: "<<num_misses<<endl;
	double rate = (double) (num_misses - num_blocks_accessed)/ total_accesses;
	cout<<"Conflict Miss Rate: "<<rate<<endl;	
	return 0;

}
