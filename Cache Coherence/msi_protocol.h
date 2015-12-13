# include "cache_coherency_protocol.h"
enum MSI {
	M,
	S,
	I,
	X
};

class MSIProtocol: public CacheCoherencyProtocol {
	// update state based on action
public:
	MSI state;
	MSIProtocol(){state = I;}
	void update_state(Action action);
	// returns true if block should be written back based on action
	bool is_wb(Action action);
	// returns true if invalidate signal should be placed on bus based on action
	bool is_invalidate(Action action);
	bool is_invalid();
};
