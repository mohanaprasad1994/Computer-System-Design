# include "msi_protocol.h"
void MSIProtocol::update_state(Action action) {
	switch(state) {
		M : switch(action) {
				case CPU_READ_HIT: {state = M;break;}
				case CPU_READ_MISS: {state = S;break;}
				case CPU_READ_ONLY_MISS: {state = S;break;}
				case CPU_WRITE_HIT: {state = M;break;}
				case CPU_WRITE_MISS: {state = M;break;}
				case READ_MISS_BUS: {state = S;break;}
				case WRITE_MISS_BUS: {state = I;break;}
				case INVALIDATE_BUS: {state = I;break;}
			}
		S : switch(action) {
				case CPU_READ_HIT: {state = S;break;}
				case CPU_READ_MISS: {state = S;break;}
				case CPU_READ_ONLY_MISS: {state = S;break;}
				case CPU_WRITE_HIT: {state = M;break;}
				case CPU_WRITE_MISS: {state = M;break;}
				case READ_MISS_BUS: {state = S;break;}
				case WRITE_MISS_BUS: {state = I;break;}
				case INVALIDATE_BUS: {state = I;break;}
			}
		I : switch(action) {
				case CPU_READ_HIT: {state = X;break;}
				case CPU_READ_MISS: {state = S;break;}
				case CPU_READ_ONLY_MISS: {state = S;break;}
				case CPU_WRITE_HIT: {state = X;break;}
				case CPU_WRITE_MISS: {state = M;break;}
				case READ_MISS_BUS: {state = I;break;}
				case WRITE_MISS_BUS: {state = I;break;}
				case INVALIDATE_BUS: {state = I;break;}
			}
	}
}

bool MSIProtocol::is_wb(Action action) {
	return (
		(state == M) &&
		(	(action == WRITE_MISS_BUS) ||
			(action == CPU_WRITE_MISS) ||
			(action == READ_MISS_BUS) ||
			(action == CPU_READ_MISS) ||
			(action == CPU_READ_ONLY_MISS) ||
			(action == INVALIDATE_BUS)
		)
	);
}

bool MSIProtocol::is_invalidate(Action action) {
	return (state != M) && (action == CPU_WRITE_MISS || action == CPU_WRITE_HIT);
}

bool MSIProtocol::is_invalid() {
	return (state == I);
}
