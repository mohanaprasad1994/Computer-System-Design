#include "TreePseudoLru.h"
#include <iostream>

using namespace std;

/*
   * Auxillary functions to traverse the LRU tree
   */
int left_child (int node) {
	return 2 * node + 1;
}

int right_child (int node) {
	return 2 * node + 2;
}

int parent (int node) {
	return (node - 1) / 2;
}

bool is_left_child (int node) {
	if (node%2 == 0) {
		return false;
	}
	else {
		return true;
	}
}

TreePseudoLru::TreePseudoLru (int sets, int ways): CacheReplacementPolicy (sets, ways) {
	vector<int> set_tree (ways_ - 1);
	for (int i = 0; i < sets; ++i) {
		pseudo_lru_tree.push_back (set_tree);
		for (int j = 0; j < ways_ - 1; ++j) {
			pseudo_lru_tree [i] .push_back (0);
		}
	}
}

int TreePseudoLru::find_victim (int set) {
	int root = 0;

	// traverse LRU tree top down based on the bits
	while (root < ways_ - 1) {
		if (pseudo_lru_tree[set][root] == 0) {
			root = left_child (root);
		}
		else {
			root = right_child (root);
		}
	}
	int victim_index = root;
	victim_index -= (ways_ - 1);
	
	update_cache_miss (set, victim_index);
	return victim_index;
}	

void TreePseudoLru::update_cache_miss (int set, int victim_index) {

	// flip all bits along the ancestor path traversed bottom up
	int node = parent (victim_index + ways_ - 1);
	while (node > 0) {
		pseudo_lru_tree [set][node] = 1 - pseudo_lru_tree [set][node];
		node = parent (node);
	}
	pseudo_lru_tree [set][node] = 1 - pseudo_lru_tree [set][node];
}

void TreePseudoLru::update_cache_hit (int set, int way) {
	int node = way + ways_ - 1;
    
	// traverse the tree from bottom up changing inverting the bits
	// in the anscestor to point away from the subtree
	bool is_left = is_left_child (node);
	node = parent (node);
	while (node > 0) {
		if (is_left) {
			if (pseudo_lru_tree [set][node] == 0) {
				pseudo_lru_tree [set][node] = 1;
			}
		}
		else {
			if (pseudo_lru_tree [set][node] == 1) {
				pseudo_lru_tree [set][node] = 0;
			}
		}
		is_left = is_left_child (node);
		node = parent (node);
	}
	if (is_left) {
		if (pseudo_lru_tree [set][node] == 0) {
			pseudo_lru_tree [set][node] = 1;
		}
	}
	else {
		if (pseudo_lru_tree [set][node] == 1) {
			pseudo_lru_tree [set][node] = 0;
		}
	}
}

