#include "Cache.h"
#include <iostream>


Set::Set (int ways): ways_ (ways) {
	for (int i = 0; i < ways_; ++i) {
		cache_line_state_.push_back (INVALID);
		tag_ .push_back (0);
	}
}

void Set::insert_block (int way, unsigned long long int tag) {
	cache_line_state_ [way] = CLEAN;
	tag_ [way] = tag;
}

bool Set::set_lookup (unsigned long long int tag, int& way) {
	for (int i = 0; i < ways_; ++i) {
		if (tag_ [i] == tag && cache_line_state_[i] != INVALID) {
			way = i;
			return true;
		}
	}
	return false;
}

void Set::print_set() {
	for (int i = 0; i < ways_; ++i) {
		cout<<tag_[i]<<" "<<cache_line_state_[i]<<endl;
	}
}

Cache::Cache() {
	size_ = 1024;
	line_size_ = 4;
	associativity_ = 4;
}

int num_bits (int power_of_2) {
	int num_bits = 0;
	while (power_of_2 > 0) {
		power_of_2 /= 2;
		num_bits++;
	}
	return num_bits - 1;
}

int Cache::get_set (unsigned long long int addr) {
	int num_offset_bits = num_bits (line_size_);
	addr = (addr>>num_offset_bits);
	int num_set_bits = num_bits (num_sets_);
	int x = 1;
	for (int i = 0; i < num_set_bits; ++i) {
		x = 2*x;
	}
	x =  x - 1;
	int set = (x & addr);
	return set;
}

unsigned long long int Cache::get_tag (unsigned long long int addr) {
	int num_offset_bits = num_bits (line_size_);
	addr = (addr>>num_offset_bits);
	int num_set_bits = num_bits (num_sets_);
	addr = (addr>>num_set_bits);
	unsigned long long int tag = addr;
	return tag;
}

void Cache::build_cache() {
	num_sets_ = size_ / (line_size_ * associativity_);
	cout<<"Number of sets is: "<<num_sets_<<endl;
	Set set (associativity_);
	for (int i = 0; i < num_sets_; ++i) {
		cache.push_back (set);
	}
}

void Cache::set_size (int size) {
	size_ = size;
}

void Cache::set_line_size (int line_size) {
	line_size_ = line_size;
}

void Cache::set_associativity (int associativity) {
	associativity_ = associativity;
}

void Cache::set_repl_policy (CacheReplacementPolicy* cache_repl_policy) {
	cache_repl_policy_ = cache_repl_policy;
}

bool Cache::cache_lookup (unsigned long long int addr) {
	int set = get_set(addr);
	unsigned long long int tag = get_tag(addr);
	int way = 0;
	bool hit = cache[set].set_lookup(tag, way);
	if (hit) {
		cache_repl_policy_->update_cache_hit(set, way);
	}
	else {
		way = cache_repl_policy_->find_victim(set);
		cache[set].insert_block(way, tag);
	}
	return hit;
}

