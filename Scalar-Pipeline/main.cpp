#include "simulator.h"

int main(int argc, char* argv[]){
	if (argc == 1) {
		cout << "Error: No input file!" << endl;
		return 0;
	}
    Simulator* simulator = new Simulator(argv[1], argv[2], false, false);
	simulator->simulate();
	delete simulator;
	return 1;
}

