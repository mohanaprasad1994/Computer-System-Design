#include "simulator.h"

#include<stdlib.h>

int get_int_from_string(string s){
	int num = 0;
	int len = s.length();
	int pow2 = 1;
	for(int i = len-1; i >= 0; i--){
		if(s[i] == '1')
			num += pow2;
		pow2 *= 2;

	}
	return num;
}

int get_twos_complement(string s) {
	int num = 0;
	if (s[0] == '0') {
		return get_int_from_string(s);
	}
	else {
		// if 2'complement of a negative no y is x then ~x + 1 = -y
		for(int i = 0; i < s.size(); ++i) {
			s[i] = '0' + (1 - (s[i] - '0'));
		}
		num = get_int_from_string(s);
		num++;
		return (-num);
	}
}

Simulator::Simulator(string input_file,string data_file, bool b_enable, bool of_enable){
	ins_index[0] = 5;//for fetch
	ins_index[1] = 4;
	ins_index[2] = 3;
	ins_index[3] = 2;
	ins_index[4] = 1;
	ins_index[5] = 0;

	for(int i=0;i<6;i++){
		ins_pipeline[i].opcode = 7;
		ins_pipeline[i].IR = "1111000000000000"; //assuming 16 bit instructions
		ins_pipeline[i].wait_for_op1 = false;
		ins_pipeline[i].wait_for_op2 = false;
		ins_pipeline[i].wait_for_op3 = false;
	}
	
	temp_decode.opcode = 7;
	temp_decode.IR = "1111000000000000";

	temp_fetch.opcode = 7;
	temp_fetch.IR = "1111000000000000";
	//decode stage
	prev_ins_decoded_is_branch = false;
	control_flag = false;
	m_clk = 0;

	pc = 0;
	for(int i = 0; i < NUM_REGISTERS; ++i) {
		register_status.push_back(-1);
		register_file[i] = 0;
		forward_file[i] = 0;
		wait_on_reg[i] = false;
	}

	register_file[0] = 0;

	operand_forwarding = false;
	raw_flag = false;
	unset_raw_flag_cycle = false;

	prev_raw_flag = false;
	input_code = input_file;
	d_cache_file = data_file;
	branch_pred_enabled = b_enable;
	operand_forward_enabled = of_enable;
	flush_pipeline = false;
	num_ins_executed = 0;
	num_stalls = 0;
	num_control_stalls = 0;



}

int Simulator::fetch(int ins_index) {
	pipeline_instr p;
	if (control_flag || raw_flag) {
		p.IR = "1111000000000000";
	}
	else {
		if (i_cache.count(pc) == 0) {
			p.IR = "1111000000000000";
		}
		else {
			p.IR = i_cache[pc];
		}
		p.pc = pc;
		pc += 2;
	}
	ins_pipeline[ins_index] = p;
	out << ins_pipeline[ins_index].IR << endl;
	return 1;
	
}


int Simulator::decode(int ins_index){

	bool is_raw = false;

	if (prev_raw_flag){
		prev_raw_flag = false;
		temp_fetch = ins_pipeline[ins_index];
		ins_pipeline[ins_index].opcode = 7;
		ins_pipeline[ins_index].IR = "1111000000000000";
	}


	if(prev_ins_decoded_is_branch){
		prev_ins_decoded_is_branch = false;
		ins_pipeline[ins_index].opcode = 7;
		ins_pipeline[ins_index].IR = "1111000000000000";
		if (!branch_pred_enabled) {
			pc -= 2;
		}
	}

	string IR = ins_pipeline[ins_index].IR;
	ins_pipeline[ins_index].opcode = get_int_from_string(IR.substr(0,3));
	ins_pipeline[ins_index].immediate = (get_int_from_string(IR.substr(3,1)) >= 1)?true:false;

	out << ins_pipeline[ins_index].IR << endl;
	if(ins_pipeline[ins_index].opcode == 7 && ins_pipeline[ins_index].immediate == 0)
		ins_pipeline[ins_index].opcode = 8;

	if (ins_pipeline[ins_index].opcode == 5){ // jump instruction
		ins_pipeline[ins_index].op1 = get_twos_complement(IR.substr(4,8));
		prev_ins_decoded_is_branch = true;
		control_flag = true;
		if (branch_pred_enabled) {
			control_flag = false;
			if (bp.count(ins_pipeline[ins_index].pc) == 0) {
				bp[ins_pipeline[ins_index].pc] = BranchPredictor();
				btb[ins_pipeline[ins_index].pc] = ins_pipeline[ins_index].pc + 2;
			}
			if (bp[ins_pipeline[ins_index].pc].predict()) {
			   pc = btb[ins_pipeline[ins_index].pc];
			}
			else {
			   prev_ins_decoded_is_branch = false;
			}
		}			
	}
	else if (ins_pipeline[ins_index].opcode == 6){ // BEQZ
		ins_pipeline[ins_index].op1 = get_int_from_string(IR.substr(4,4));
		ins_pipeline[ins_index].op2 = get_twos_complement(IR.substr(8,8));
		prev_ins_decoded_is_branch = true;
		control_flag = true;
		
		if (branch_pred_enabled) {
			control_flag = false;
			if (bp.count(ins_pipeline[ins_index].pc) == 0) {
				bp[ins_pipeline[ins_index].pc] = BranchPredictor();
				btb[ins_pipeline[ins_index].pc] = ins_pipeline[ins_index].pc + 2;
			}
			if (bp[ins_pipeline[ins_index].pc].predict()) {
			   pc = btb[ins_pipeline[ins_index].pc];
			}
			else {
			   prev_ins_decoded_is_branch = false;
			}
		}			
		if (register_status[ins_pipeline[ins_index].op1] != -1){
			is_raw = true;
			wait_on_reg[ins_pipeline[ins_index].op1] = true;
			ins_pipeline[ins_index].wait_for_op1 = true;
		}

	}
	else if (ins_pipeline[ins_index].opcode <= 4) { // add, sub, mul, ld,st
		ins_pipeline[ins_index].op1 = get_int_from_string(IR.substr(4,4));
		ins_pipeline[ins_index].op2 = get_int_from_string(IR.substr(8,4));
		if (ins_pipeline[ins_index].immediate) {
			ins_pipeline[ins_index].op3 = get_twos_complement(IR.substr(12,4));
		}
		else {
			ins_pipeline[ins_index].op3 = get_int_from_string(IR.substr(12,4));
		}


		if( (ins_pipeline[ins_index].opcode <= 2 && ins_pipeline[ins_index].immediate) || ins_pipeline[ins_index].opcode == 3 )
			if (register_status[ins_pipeline[ins_index].op2] != -1){
				is_raw = true;
				ins_pipeline[ins_index].wait_for_op2 = true;
				wait_on_reg[ins_pipeline[ins_index].op2] = true;
			}

		if( (ins_pipeline[ins_index].opcode <=2 && !ins_pipeline[ins_index].immediate) ){
			if (register_status[ins_pipeline[ins_index].op2] != -1){
				wait_on_reg[ins_pipeline[ins_index].op2] = true;
				is_raw = true;
				ins_pipeline[ins_index].wait_for_op2 = true;
			} 
			if (register_status[ins_pipeline[ins_index].op3] != -1){
				wait_on_reg[ins_pipeline[ins_index].op3] = true;
				is_raw = true;
				ins_pipeline[ins_index].wait_for_op3 = true;
			}
		}



		if(ins_pipeline[ins_index].opcode == 4) {

			if (register_status[ins_pipeline[ins_index].op1] != -1){
				wait_on_reg[ins_pipeline[ins_index].op1] = true;
				is_raw = true;
				ins_pipeline[ins_index].wait_for_op1 = true;
			}
			if (register_status[ins_pipeline[ins_index].op2] != -1){
				wait_on_reg[ins_pipeline[ins_index].op2] = true;
				is_raw = true;
				ins_pipeline[ins_index].wait_for_op2 = true;
			}
		}

	}

	if (is_raw){
		raw_flag = true;
		prev_raw_flag = true;
		temp_decode = ins_pipeline[ins_index];
		ins_pipeline[ins_index].opcode = 7;
		ins_pipeline[ins_index].IR = "1111000000000000";
	}

	else{

		if(ins_pipeline[ins_index].opcode <= 3)
			register_status[ins_pipeline[ins_index].op1] = ins_pipeline[ins_index].pc;
	}



	return 1;
}


int Simulator::register_read(int ins_index) {

	if (operand_forward_enabled){
		if (ins_pipeline[ins_index].opcode <= 2) {
			ins_pipeline[ins_index].A = (ins_pipeline[ins_index].wait_for_op2)?forward_file[ins_pipeline[ins_index].op2]:register_file[ins_pipeline[ins_index].op2];
			ins_pipeline[ins_index].wait_for_op2 = false;
			if(ins_pipeline[ins_index].immediate) {
				ins_pipeline[ins_index].imm_field = ins_pipeline[ins_index].op3;
			}
			else {
				ins_pipeline[ins_index].B = (ins_pipeline[ins_index].wait_for_op3)?forward_file[ins_pipeline[ins_index].op3]:register_file[ins_pipeline[ins_index].op3];
				ins_pipeline[ins_index].wait_for_op3 = false;
			}
		}
		else if (ins_pipeline[ins_index].opcode == 3) {
			ins_pipeline[ins_index].A = (ins_pipeline[ins_index].wait_for_op2)?forward_file[ins_pipeline[ins_index].op2]:register_file[ins_pipeline[ins_index].op2];
			ins_pipeline[ins_index].wait_for_op2 = false;
		}
		else if (ins_pipeline[ins_index].opcode == 4) {
			ins_pipeline[ins_index].A = (ins_pipeline[ins_index].wait_for_op1)?forward_file[ins_pipeline[ins_index].op1]:register_file[ins_pipeline[ins_index].op1];
			ins_pipeline[ins_index].wait_for_op1 = false;
			ins_pipeline[ins_index].B = (ins_pipeline[ins_index].wait_for_op2)?forward_file[ins_pipeline[ins_index].op2]:register_file[ins_pipeline[ins_index].op2];
			ins_pipeline[ins_index].wait_for_op2 = false;
		}
		else if(ins_pipeline[ins_index].opcode == 5) {
			ins_pipeline[ins_index].imm_field = ins_pipeline[ins_index].op1;
		}
		else if(ins_pipeline[ins_index].opcode == 6) {
			ins_pipeline[ins_index].A = (ins_pipeline[ins_index].wait_for_op1)?forward_file[ins_pipeline[ins_index].op1]:register_file[ins_pipeline[ins_index].op1];
			ins_pipeline[ins_index].wait_for_op1 = false;
			ins_pipeline[ins_index].imm_field = ins_pipeline[ins_index].op2;
		}
		out << ins_pipeline[ins_index].IR << endl;
		return 1;
	}
	if (ins_pipeline[ins_index].opcode <= 2) {
		ins_pipeline[ins_index].A = register_file[ins_pipeline[ins_index].op2];
		if(ins_pipeline[ins_index].immediate) {
			ins_pipeline[ins_index].imm_field = ins_pipeline[ins_index].op3;
		}
		else {
			ins_pipeline[ins_index].B = register_file[ins_pipeline[ins_index].op3];
		}
	}
	else if (ins_pipeline[ins_index].opcode == 3) {
		ins_pipeline[ins_index].A = register_file[ins_pipeline[ins_index].op2];
	}
	else if (ins_pipeline[ins_index].opcode == 4) {
		ins_pipeline[ins_index].A = register_file[ins_pipeline[ins_index].op1];
		ins_pipeline[ins_index].B = register_file[ins_pipeline[ins_index].op2];
	}
	else if(ins_pipeline[ins_index].opcode == 5) {
		ins_pipeline[ins_index].imm_field = ins_pipeline[ins_index].op1;
	}
	else if(ins_pipeline[ins_index].opcode == 6) {
		ins_pipeline[ins_index].A = register_file[ins_pipeline[ins_index].op1];
		ins_pipeline[ins_index].imm_field = ins_pipeline[ins_index].op2;
	}
	out << ins_pipeline[ins_index].IR << endl;
	return 1;
}	

int Simulator::execute (int ins_index){
	long long int A = ins_pipeline[ins_index].A;
	long long int B = ins_pipeline[ins_index].B;
	long long int imm_field = ins_pipeline[ins_index].imm_field;

	switch(ins_pipeline[ins_index].opcode){

		case 0: if (ins_pipeline[ins_index].immediate) ins_pipeline[ins_index].alu_output = A + imm_field;
					else ins_pipeline[ins_index].alu_output = A + B; break;
		case 1: if (ins_pipeline[ins_index].immediate) ins_pipeline[ins_index].alu_output = A - imm_field;
					else ins_pipeline[ins_index].alu_output = A - B; break;
		case 2: if (ins_pipeline[ins_index].immediate) ins_pipeline[ins_index].alu_output = A * imm_field;
					else ins_pipeline[ins_index].alu_output = A * B; break;

		case 3: ins_pipeline[ins_index].alu_output = 0 + A; break;
		case 4: ins_pipeline[ins_index].alu_output = 0 + A; break;

		case 5: ins_pipeline[ins_index].alu_output = ins_pipeline[ins_index].pc + (imm_field*2); ins_pipeline[ins_index].cond = 1; break;
		case 6: ins_pipeline[ins_index].alu_output = ins_pipeline[ins_index].pc + (imm_field*2); ins_pipeline[ins_index].cond = (A == 0); break;
	}
	out << ins_pipeline[ins_index].IR << endl;
	return 1;
}

int Simulator::mem_branch_cycle (int ins_index) {
	if (ins_pipeline[ins_index].opcode == 3) {
		if (d_cache.count (ins_pipeline[ins_index].alu_output) > 0) {
			ins_pipeline[ins_index].load_md = d_cache[ins_pipeline[ins_index].alu_output];
		}
		else {
			ins_pipeline[ins_index].load_md = 0;
		}
	}
	else if (ins_pipeline[ins_index].opcode == 4) {
		d_cache[ins_pipeline[ins_index].alu_output] = ins_pipeline[ins_index].B;
	}
	else if (ins_pipeline[ins_index].opcode == 5) {
		long long int temp_pc;
		if (branch_pred_enabled) {
			temp_pc = pc;
		}
		pc = ins_pipeline[ins_index].alu_output;
		control_flag = false;
		if(branch_pred_enabled) {
			if (!bp[ins_pipeline[ins_index].pc].predict()) {
				flush_pipeline = true;
				btb[ins_pipeline[ins_index].pc] = pc;
			}
			else {
				pc = temp_pc;
			}
			bp[ins_pipeline[ins_index].pc].update_state(true);
		}
	}
	else if (ins_pipeline[ins_index].opcode == 6) {
		control_flag = false;
		if (ins_pipeline[ins_index].cond) {
			long long int temp_pc;
			if (branch_pred_enabled) {
				temp_pc = pc;
			}
			pc = ins_pipeline[ins_index].alu_output;
			if(branch_pred_enabled) {
				if (!bp[ins_pipeline[ins_index].pc].predict()) {
					flush_pipeline = true;
					btb[ins_pipeline[ins_index].pc] = pc;
				}
				else {
					pc = temp_pc;
				}
				bp[ins_pipeline[ins_index].pc].update_state(true);
			}
		}
		else {
			if(branch_pred_enabled) {
				if (bp[ins_pipeline[ins_index].pc].predict()) {
					flush_pipeline = true;
					pc = ins_pipeline[ins_index].pc + 2;
				}
				bp[ins_pipeline[ins_index].pc].update_state(false);
			}
		}
	}
	out << ins_pipeline[ins_index].IR << endl;
	if (operand_forward_enabled){

		if (ins_pipeline[ins_index].opcode <=2)
			forward_file[ins_pipeline[ins_index].op1] = ins_pipeline[ins_index].alu_output;
		if (ins_pipeline[ins_index].opcode == 3)
			forward_file[ins_pipeline[ins_index].op1] = ins_pipeline[ins_index].load_md;

		if (ins_pipeline[ins_index].opcode <= 3)
			if(register_status[ins_pipeline[ins_index].op1] == ins_pipeline[ins_index].pc){
				register_status[ins_pipeline[ins_index].op1] = -1;
				operand_forwarding = true;
			}
		
		if (raw_flag){
			bool raw_not_over = false;
			for(int i =0; i <NUM_REGISTERS; i++){
				if(wait_on_reg[i] == true && register_status[i] == -1)
					wait_on_reg[i] = false;
				else if (wait_on_reg[i] == true && register_status[i] != -1)
					raw_not_over = true;

			}

			if(!raw_not_over){
				raw_flag = false;
				if(temp_decode.opcode <=3)
					register_status[temp_decode.op1] = temp_decode.pc;
				unset_raw_flag_cycle = true;

			}
		}

	}
	return 1;
}

int Simulator::write_back(int ins_index){
	if (ins_pipeline[ins_index].opcode <=2)
		register_file[ins_pipeline[ins_index].op1] = ins_pipeline[ins_index].alu_output;
	if (ins_pipeline[ins_index].opcode == 3)
		register_file[ins_pipeline[ins_index].op1] = ins_pipeline[ins_index].load_md;


	if (ins_pipeline[ins_index].opcode <= 3){
		if(register_status[ins_pipeline[ins_index].op1] == ins_pipeline[ins_index].pc) {
			register_status[ins_pipeline[ins_index].op1] = -1;
		}
	}

	if (raw_flag){
		bool raw_not_over = false;
		for(int i =0; i <NUM_REGISTERS; i++){
			if(wait_on_reg[i] == true && register_status[i] == -1)
				wait_on_reg[i] = false;
			else if (wait_on_reg[i] == true && register_status[i] != -1)
				raw_not_over = true;

		}

		if(!raw_not_over){
			raw_flag = false;
			if(temp_decode.opcode <=3)
				register_status[temp_decode.op1] = temp_decode.pc;
			unset_raw_flag_cycle = true;

		}
	}
	out << ins_pipeline[ins_index].IR << endl;
	if (ins_pipeline[ins_index].opcode != 7)
		num_ins_executed++;
	else
		num_stalls++;

	if (ins_pipeline[ins_index].opcode == 5 || ins_pipeline[ins_index].opcode == 6 )
		num_control_stalls += 4;
	if (ins_pipeline[ins_index].opcode == 8)
		return 0;
	return 1;
}

void Simulator::flush() {
	for (int i = 0; i < 4; ++i) {
		ins_pipeline[ins_index[i]].opcode = 7;
		ins_pipeline[ins_index[i]].IR = "1111000000000000"; //assuming 16 bit instructions
	}
	for (int i = 0; i < NUM_REGISTERS; ++i) {
		register_status[i] = -1;
		wait_on_reg[i] = false;
		raw_flag = false;
	}
	flush_pipeline = false;
}

int Simulator::simulate(){
	load_i_cache();
	load_d_cache();
	print_d_cache();
	print_i_cache();
	while(1){
		out.open("ins_pipeline.txt",ios::out);
		fetch(ins_index[0]);
		decode(ins_index[1]);
		register_read(ins_index[2]);
		execute(ins_index[3]);
		mem_branch_cycle(ins_index[4]);
		if (!write_back(ins_index[5])) {
			cout << "Simulation Ends!" << endl;
			break;
		}
		if(unset_raw_flag_cycle) {
			unset_raw_flag_cycle = false;
			if (!control_flag && !prev_raw_flag) {
				ins_pipeline[ins_index[0]] = temp_fetch;
			}
			prev_raw_flag = false;
			ins_pipeline[ins_index[1]] = temp_decode;
		}
		if(branch_pred_enabled) {
			if (flush_pipeline) {
				flush();
			}
		}
		out << (m_clk + 1) << endl;
		out.close();
		system ("python gui.py");
		next_clock_cycle();
		//print_reg_file();
		
	}
	print_d_cache();
	printf("clk cycles = %lld\n",m_clk + 1);
	printf("CPI = %f\n",1.0*(m_clk + 1)/num_ins_executed);
	printf("Stalls = %lld\n",num_stalls - 5);
	if (!branch_pred_enabled){
		printf("Control Stalls = %lld\n",num_control_stalls);
		printf("RAW Stalls = %lld\n",num_stalls - 5 - num_control_stalls);
	}
	return 1;
}

void Simulator::load_d_cache() {
	ifstream in;
	in.open(d_cache_file.c_str(),ios::in);
	long long int addr= 0;
	int val;
	while (!in.eof()) {
		in>>addr>>val;
		d_cache[addr] = val;

	}

	in.close();
}

void Simulator::load_i_cache() {
	ifstream in;
	in.open(input_code.c_str(),ios::in);
	long long int pc_value = 0;
	string instr;
	while (getline(in, instr)) {
		if (instr == "1110000000000000") {
			i_cache[pc_value] = instr;
			pc_value += 2;
			for (int i = 0; i < 5; ++i) {
				i_cache[pc_value] = "1111000000000000";
				pc_value += 2;
			}
		}
		else {
			i_cache[pc_value] = instr;
			pc_value += 2;
		}
	}
	in.close();
}

void Simulator::print_i_cache() {
	cout << "I Cache Size: " << i_cache.size() << endl;
	for(map<long long int, string>::iterator it = i_cache.begin(); it != i_cache.end(); ++it) {
		cout << it->first << ": " << it->second <<endl;
	}
}

void Simulator::print_d_cache() {
	cout << "D Cache\n";
	cout << d_cache.size() << endl;
	for(map<int, int>::iterator it = d_cache.begin(); it != d_cache.end(); ++it) {
		cout << it->first << ": " << it->second <<endl;
	}
}

void Simulator::print_reg_file() {
	cout << "Reg File\n";
	for (int i = 0; i < NUM_REGISTERS; ++i) {
		cout << i << ": " << register_file[i] << endl;
	}
	cout << "Wait on Reg\n";
	for (int i = 0; i < NUM_REGISTERS; ++i) {
		cout << i << ": " << wait_on_reg[i] << endl;
	}

	cout << "Reg Status\n";
	for (int i = 0; i < NUM_REGISTERS; ++i) {
		cout << i << ": " << register_status[i] << endl;
	}
}
