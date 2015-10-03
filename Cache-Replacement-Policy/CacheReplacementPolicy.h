#ifndef CacheReplacementPolicy_H
#define CacheReplacementPolicy_H

// This class defines the virtual interface for cache replacement policy
class CacheReplacementPolicy {
  protected:
 	int ways_;
	int sets_;

  public:
	CacheReplacementPolicy(int sets, int ways): sets_ (sets), ways_ (ways) {}
	virtual ~CacheReplacementPolicy() {}
	virtual void update_cache_miss(int set,int victim_index) = 0;
	virtual void update_cache_hit(int set, int way) = 0;
	virtual int find_victim(int set) = 0;
};

#endif
