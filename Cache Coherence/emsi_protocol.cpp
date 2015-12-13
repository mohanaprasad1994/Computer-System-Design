# include "emsi_protocol.h"

void EMSIProtocol::update_state(Action action) {
	switch(state) {
		Ex : switch(action) {
				case CPU_READ_HIT: {state = Ex;break;}//
				case CPU_READ_MISS: {state = Sh;break;}//
				case CPU_READ_ONLY_MISS: {state = Ex;break;}//
				case CPU_WRITE_HIT: {state = Mo;break;}//
				case CPU_WRITE_MISS: {state = Mo;break;}//
				case READ_MISS_BUS: {state = Sh;break;}//
				case WRITE_MISS_BUS: {state = In;break;}//
				case INVALIDATE_BUS: {state = In;break;}//
			}
		Mo : switch(action) {
				case CPU_READ_HIT: {state = Mo;break;}//
				case CPU_READ_MISS: {state = Sh;break;}//
				case CPU_READ_ONLY_MISS: {state = Ex;break;}//
				case CPU_WRITE_HIT: {state = Mo;break;}//
				case CPU_WRITE_MISS: {state = Mo;break;}//
				case READ_MISS_BUS: {state = Sh;break;}//
				case WRITE_MISS_BUS: {state = In;break;}//
				case INVALIDATE_BUS: {state = In;break;}//
			}
		Sh : switch(action) {
				case CPU_READ_HIT: {state = Sh;break;}//
				case CPU_READ_MISS: {state = Sh;break;}//
				case CPU_READ_ONLY_MISS: {state = Ex;break;}//
				case CPU_WRITE_HIT: {state = Mo;break;}//
				case CPU_WRITE_MISS: {state = Mo;break;}//
				case READ_MISS_BUS: {state = Sh;break;}//
				case WRITE_MISS_BUS: {state = In;break;}//
				case INVALIDATE_BUS: {state = In;break;}//
			}
		In : switch(action) {
				case CPU_READ_HIT: {state = X1;break;}
				case CPU_READ_MISS: {state = Sh;break;}
				case CPU_READ_ONLY_MISS: {state = Ex;break;}//
				case CPU_WRITE_HIT: {state = X1;break;}
				case CPU_WRITE_MISS: {state = Mo;break;}//
				case READ_MISS_BUS: {state = In;break;}//
				case WRITE_MISS_BUS: {state = In;break;}//
				case INVALIDATE_BUS: {state = In;break;}//
			}
	}
}

bool EMSIProtocol::is_wb(Action action) {
	return (
		(state == Mo) &&
		(	(action == WRITE_MISS_BUS) ||
			(action == CPU_WRITE_MISS) ||
			(action == READ_MISS_BUS) ||
			(action == CPU_READ_MISS) ||
			(action == CPU_READ_ONLY_MISS) ||
			(action == INVALIDATE_BUS)
		)
	);
}

bool EMSIProtocol::is_invalidate(Action action) {
	return (state != Mo) && (action == CPU_WRITE_MISS || action == CPU_WRITE_HIT);
}

bool EMSIProtocol::is_invalid() {
	return (state == In);
}
