#include <map>
#include <vector>
#include <stdlib.h>

//#include "cache_coherency_protocol.h"
#include "msi_protocol.h"
using namespace std;
// #include "emsi_protocol.h"
class Bus;

class Core{
	int index;
	int num_lines;
	int line_size;
	int size;
	vector<bool> cache;
	vector<int> tags;
	vector<CacheCoherencyProtocol*> cc_proto;

  public:
	Core(int id) {
		index = id;
		size = 1024;
		line_size = 32;
		num_lines = size/line_size;
		for (int i = 0; i < num_lines; ++i) {
			cache.push_back(false);
			tags.push_back(0);
			cc_proto.push_back(new MSIProtocol());
		}
	}
	void read(int addr, Bus &bus);
	void write(int addr, Bus &bus);
	bool cache_hit(int addr, int& set);

	void invalidate_signal(int block);
	void read_miss_bus(int block, Bus& bus);
	void write_miss_bus(int block, Bus& bus);

};

class Bus{
	vector<Core>* cores;
	int *m_clk;
  public:
     Bus(vector<Core>* c, int* clk)
	 {
		 cores = c;
		 m_clk = clk;
	 }
	 void place_read_miss_bus(int block, int id);
	 void place_write_miss_bus(int block, int id);
	 void place_invalidate_bus(int block, int id);
	 void read_block_bus();
	 void write_block_bus();
	 bool check_exclusive(int addr, int id);
};
