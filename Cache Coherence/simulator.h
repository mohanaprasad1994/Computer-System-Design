#include "core.h"
#define NUM_CORES 2
#define MAX_CYCLE 1000

class Simulator {
	vector<Core> cores;
	Bus* bus;
	int m_clk;

  public:
	Simulator() {
		m_clk = 0;
	    for(int i = 0; i < NUM_CORES; ++i)
		{
			cores.push_back(Core(i));
		}
		bus = new Bus(&cores, &m_clk);
	}
	void simulate();
};
