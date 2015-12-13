#include "core.h"

void Core::read(int addr, Bus& bus) {
	int set = 0;
	bool read_hit = cache_hit(addr, set);
	if (!read_hit) {
		if (bus.check_exclusive(addr, index) == true) {
			if (cc_proto[set]->is_wb(CPU_READ_ONLY_MISS) == true) {
				bus.write_block_bus ();
			}
			bus.place_read_miss_bus (addr/32, index);
			bus.read_block_bus ();
			tags[set] = addr/32;
			cc_proto[set]->update_state(CPU_READ_ONLY_MISS);

		}
		else
		{
			if (cc_proto[set]->is_wb(CPU_READ_MISS) == true) {
				bus.write_block_bus ();
			}
			bus.place_read_miss_bus (addr/32, index);
			bus.read_block_bus ();
			tags[set] = addr/32;
			cc_proto[set]->update_state(CPU_READ_MISS);
		}
	}
	if (read_hit) {
		cc_proto[set]->update_state(CPU_READ_HIT);
	}
}

void Core::write(int addr, Bus& bus) {
	int set = 0;
	bool write_hit = cache_hit(addr, set);
	if (!write_hit) {
		if (cc_proto[set]->is_wb(CPU_WRITE_MISS) == true) {
			bus.write_block_bus ();
		}
		bus.place_write_miss_bus (addr, index);
		bus.write_block_bus ();
		tags[set] = addr/32;
		cc_proto[set]->update_state(CPU_WRITE_MISS);
	}
	if (write_hit) {
		if (cc_proto[set]->is_invalidate(CPU_READ_HIT) == true) {
			bus.place_invalidate_bus (tags[set], index);
		}
		cc_proto[set]->update_state(CPU_WRITE_HIT);
	}
}

void Core::invalidate_signal (int block) {
	int set = 0;
	bool hit = cache_hit (block*32, set);
	if (hit) {
		cc_proto[set]->update_state (INVALIDATE_BUS);
	}
}

void Core::write_miss_bus (int block, Bus& bus) {
	int set = 0;
	bool hit = cache_hit (block*32, set);
	if (hit) {
		if (cc_proto[set]->is_wb(WRITE_MISS_BUS) == true) {
			bus.write_block_bus ();
		}
		cc_proto[set]->update_state (WRITE_MISS_BUS);
	}
}

void Core::read_miss_bus (int block, Bus& bus) {
	int set = 0;
	bool hit = cache_hit (block*32, set);
    if (hit) {
		if (cc_proto[set]->is_wb(READ_MISS_BUS) == true) {
			bus.write_block_bus ();
		}
		cc_proto[set]->update_state (READ_MISS_BUS);
	}
}

bool Core::cache_hit (int addr, int &set) {
	addr =  addr/32;
	set = addr%num_lines;
	if (!cc_proto[set]->is_invalid() && tags[set] == addr) {
		return true;
	}
	else {
		return false;
	}
}

void Bus::place_read_miss_bus(int block, int id) {
	for(int i = 0; i < cores->size(); ++i) {
		if (i != id) {
			(*cores)[i].read_miss_bus(block, (*this));
		}
	}
}

void Bus::place_write_miss_bus(int block, int id) {
	for(int i = 0; i < cores->size(); ++i) {
		if (i != id) {
			(*cores)[i].write_miss_bus(block, (*this));
		}
	}
}

void Bus::place_invalidate_bus(int block, int id) {
	for(int i = 0; i < cores->size(); ++i) {
		if (i != id) {
			(*cores)[i].invalidate_signal(block);
		}
	}
}

void Bus::read_block_bus() {
	m_clk += 8;
}

void Bus::write_block_bus() {
	m_clk += 8;
}

bool Bus::check_exclusive(int addr, int id) {
	int set;
	bool flag = true;
	for(int i = 0; i < cores->size(); ++i) {
		if (i != id) {
			if ((*cores)[i].cache_hit(addr, set) == true) {
				flag = false;
				break;
			}
		}
	}
	return flag;
}



