
enum Action { 
	CPU_READ_HIT,
	CPU_READ_MISS,
	CPU_READ_ONLY_MISS,
	CPU_WRITE_HIT,
	CPU_WRITE_MISS,
	READ_MISS_BUS,
	WRITE_MISS_BUS,
	INVALIDATE_BUS
};

// CacheCoherencyProtocol defines an interface
class CacheCoherencyProtocol {
  public:
	// update state based on action
	virtual void update_state(Action action) = 0;
	// returns true if block should be written back based on action
	virtual bool is_wb(Action action) = 0;
	// returns true if invalidate signal should be placed on bus based on action
	virtual bool is_invalidate(Action action) = 0;

	virtual bool is_invalid ();
};
