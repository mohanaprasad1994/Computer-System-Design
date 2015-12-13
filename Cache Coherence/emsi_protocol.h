# include "cache_coherency_protocol.h"

enum EMSI {
	Ex,
	Mo,
	Sh,
	In,
	X1
};

class EMSIProtocol: public CacheCoherencyProtocol {
	// update state based on action
public:
	EMSI state;
	EMSIProtocol(){state = In;}
	void update_state(Action action);
	// returns true if block should be written back based on action
	bool is_wb(Action action);
	// returns true if invalidate signal should be placed on bus based on action
	bool is_invalidate(Action action);
	bool is_invalid();
};
