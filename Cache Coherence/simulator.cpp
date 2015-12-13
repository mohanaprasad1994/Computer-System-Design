#include "simulator.h"

#include <stdlib.h>
#include <time.h>

void Simulator::simulate() {
	int curr_ind = 0;
	time_t t;
	srand((unsigned) time(&t));
	while (m_clk < MAX_CYCLE) {
		int block = rand()%1024;
		int addr = block*32;
		cores[curr_ind].read (addr, *bus);
		int write_block = rand()%8;
		int write_addr = addr + write_block*4;
		cores[curr_ind].write (write_addr, *bus);
		curr_ind = (curr_ind + 1)%2;
	}
	m_clk++;
}



