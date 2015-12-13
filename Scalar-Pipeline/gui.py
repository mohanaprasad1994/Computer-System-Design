#!/usr/bin/python
from Tkinter import *
import Tkinter
top = Tkinter.Tk()
# Code to add widgets will go here...
def get_int_from_string( s):
	num = 0
	leng = len(s)
	pow2 = 1
	i = leng - 1
	while i >= 0:
		if(s[i] == '1'):
			num += pow2
		pow2 *= 2
		i -= 1
	
	return num


def get_twos_complement(s):
	num = 0
	t = ""
	if (s[0] == '0'): 
		return get_int_from_string(s);
	
	else:
		# if 2'complement of a negative no y is x then ~x + 1 = -y
		for i in range(len(s)):
			if s[i] == '1':
				t += '0'
			else:
				t += '1'
			
		
		num = get_int_from_string(t)
		num += 1
		return (-1*num)



fil = open("ins_pipeline.txt",'r')
lines = []
for line in fil:
	lines.append(line.strip())
op = []
op1 = []
op2 = []
op3 = []
for line in lines:
	s = line
	
	if line[:3] == "000":
		op.append("ADD ")
		op1.append("R"+str(get_int_from_string(s[4:8])))
		op2.append("R"+str(get_int_from_string(s[8:12])))
		
		if line[3] == '1':
			op3.append("#"+str(get_twos_complement(s[12:16])))
		else:
			op3.append("R"+str(get_int_from_string(s[12:16])))


	elif line[:3] == "001":
		op.append("SUB ")
		op1.append("R"+str(get_int_from_string(s[4:8])))
		op2.append("R"+str(get_int_from_string(s[8:12])))
		if line[3] == '1':
			op3.append("#"+str(get_twos_complement(s[12:16])))
		else:
			op3.append("R"+str(get_int_from_string(s[12:16])))
	elif line[:3] == "010":
		op.append("MUL ")
		op1.append("R"+str(get_int_from_string(s[4:8])))
		op2.append("R"+str(get_int_from_string(s[8:12])))
		if line[3] == '1':
			op3.append("#"+str(get_twos_complement(s[12:16])))
		else:
			op3.append("R"+str(get_int_from_string(s[12:16])))

	elif line[:3] == "011":
		op.append("LD ")
		op1.append("R"+str(get_int_from_string(s[4:8])))
		op2.append("[R"+str(get_int_from_string(s[8:12]))+"]")
		op3.append(" ")
	elif line[:3] == "100":
		op.append("ST ")
		op1.append("[R"+str(get_int_from_string(s[4:8]))+"]")
		op2.append("R"+str(get_int_from_string(s[8:12])))
		op3.append(" ")
	elif line[:3] == "101":
		op.append("JMP ")
		op1.append(str(get_twos_complement(s[4:12])))
		op2.append(" ")
		op3.append(" ")
	elif line[:3] == "110":
		op.append("BEQZ ")
		op1.append("R"+str(get_int_from_string(s[4:8])))
		op2.append(str(get_twos_complement(s[8:16])))
		op3.append(" ")
	elif line[:4] == "1110":
		op.append("HALT ")
		op1.append(" ")
		op2.append(" ")
		op3.append(" ")
	elif line[:4] == "1111":
		op.append("NOOP ")
		op1.append(" ")
		op2.append(" ")
		op3.append(" ")



var = StringVar()
label = Label( top, textvariable=var, relief=RAISED )

var.set("Clock Cycle = "+lines[6])
label.pack()

F = Tkinter.Button(top, text ="Fetch: " + op[0] + " "+op1[0]+" "+op2[0]+" "+op3[0])
D = Tkinter.Button(top, text ="Decode: " + op[1] + " "+op1[1]+" "+op2[1]+" "+op3[1])
R = Tkinter.Button(top, text ="Register Read: " + op[2] + " "+op1[2]+" "+op2[2]+" "+op3[2])
E = Tkinter.Button(top, text ="Execute: " + op[3] + " "+op1[3]+" "+op2[3]+" "+op3[3])
M = Tkinter.Button(top, text ="Memory Access: " + op[4] + " "+op1[4]+" "+op2[4]+" "+op3[4])
W = Tkinter.Button(top, text ="Write Back: " + op[5] + " "+op1[5]+" "+op2[5]+" "+op3[5])

F.pack()
D.pack()
R.pack()
E.pack()
M.pack()
W.pack()

top.mainloop()