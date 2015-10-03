#include <vector>

#include "CacheReplacementPolicy.h"

#ifndef Cache_H
#define Cache_H

using namespace std;

// represents the various states of a cache line
enum CacheLineState {
	INVALID, 
	CLEAN,
	DIRTY
};

// This is the class which implements functionalities of a Set
class Set {
	int ways_;

	// set - array of ways
	vector<CacheLineState> cache_line_state_;
    vector<unsigned long long int> tag_;
  
  public:
    Set(int ways);

	// looks up an addr in the Set and returns true on a hit
	bool set_lookup(unsigned long long int tag, int& way);

	// inserts the block into the set given the way
	void insert_block(int way, unsigned long long int tag);

	// prints contents of the Set
	void print_set();
};

// This is the class which implements the functionalities of a cache
class Cache {
	int size_;
	int line_size_;
	int associativity_;
	int num_sets_;
	CacheReplacementPolicy* cache_repl_policy_;

	// cache - array of Sets
    vector<Set> cache;

	// get the set from an address
	int get_set (unsigned long long int addr);

	// get the tag from an address
	unsigned long long int get_tag (unsigned long long int addr);

  public:
	Cache();

	// functions to set different parameters of the cache and build it
	void set_size(int size);
	void set_line_size(int line_size);
	void set_associativity(int a);
	void set_repl_policy (CacheReplacementPolicy* cache_repl_policy);
	void build_cache();
	
	// lookups the addr and returns true on a hit
	bool cache_lookup(unsigned long long int addr);
};

#endif
