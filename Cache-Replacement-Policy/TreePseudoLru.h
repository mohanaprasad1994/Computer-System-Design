#include "CacheReplacementPolicy.h"

#include<vector>

#ifndef TreePseudoLru_H
#define TreePseudoLru_H

using namespace std;

class TreePseudoLru: public CacheReplacementPolicy {

	// maintain a LRU tree for every set
	// implicitly represented by an array of size num_ways - 1
	vector<vector<int> > pseudo_lru_tree;

  public:
	// initialise LRU tree for each set and given no of ways
	TreePseudoLru (int sets, int ways);
	virtual ~TreePseudoLru() {}

	// update the LRU tree for corresponding set on a cache miss
	// given the victim index
	virtual void update_cache_miss(int set, int victim_index);

	// update the LRU tree for corresponding set on a cache hit
	// given way of the hit block
	virtual void update_cache_hit(int set, int way);

	// find a victim index for the given set
	virtual int find_victim(int set);
};

#endif
