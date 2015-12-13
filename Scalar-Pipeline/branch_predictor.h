
class BranchPredictor {
	int curr_state;

  public:
	BranchPredictor() {
		curr_state = 0;
	}
	
	void update_state(bool true_pred) {
		switch (curr_state) {
			case 0: if (true_pred) 
						curr_state = 1; 
					break;
			case 1: if (!true_pred) 
						curr_state = 0; 
					else 
						curr_state = 2;
					break;
			case 2: if (true_pred) 
						curr_state = 3; 
					else
						curr_state = 1;
					break;
			case 3: if (!true_pred) 
						curr_state = 2; 
					break;
		}
	}
	
	bool predict() {
		if(curr_state <= 1) {
			return false;
		}
		else {
			return true;
		}
	}
};
