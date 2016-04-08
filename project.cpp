//William T. Brown
//CSCE 2610 Project
#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include <stdlib.h>

using namespace std;

int main(){
	string cmds[100];
	string mem[16383];
	int pc = 0;
	int R[32];
	ifstream file;
	int count, memstart;
	string filen;
	string line;
	string buffer1 = "";
	string buffer2, opcode, rs, rt, rd, shamt, funct, immediate, address;
	int hex[4] = {1, 16, 256, 4096};
	int immed[16] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, -32768};
	int addr[26] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 33554432};
	int rdval, rsval, rtval, immval, addval, shamtval;

	cout << "Please enter a file name followed by .txt: ";
	cin >> filen;
	file.open(filen.c_str());
	for( int c = 0; c < 16383; c++)
		mem[c] = "";
	for(int c = 0; c < 32; c++)
		R[c] = 0;
	
	count = 0;
	while(!file.eof()){//reads all the lines of the file, and puts them into a command array; keeps track of amount of commands
		getline(file, line);
		cmds[count] = line;
		count++;
	}
	
	for(int c = 1, j = 3; c < 6 && j >= 0; c++, j--){//converts the starting address from hex into decimal
		buffer1 = cmds[0][c];
		if(buffer1 == "F" | buffer1 == "f")
			pc += (hex[j] * 15);
		else if(buffer1 == "E" | buffer1 == "e")
			pc += (hex[j] * 14);
		else if(buffer1 == "D" | buffer1 == "d")
			pc += (hex[j] * 13);
		else if(buffer1 == "C" | buffer1 == "c")
			pc += (hex[j] * 12);
		else if(buffer1 == "B" | buffer1 == "b")
			pc += (hex[j] * 11);
		else if(buffer1 == "A" | buffer1 == "a")
			pc += (hex[j] * 10);
		else if(buffer1 == "9")
			pc += (hex[j] * 9);
		else if(buffer1 == "8")
			pc += (hex[j] * 8);
		else if(buffer1 == "7")
			pc += (hex[j] * 7);
		else if(buffer1 == "6")
			pc += (hex[j] * 6);
		else if(buffer1 == "5")
			pc += (hex[j] * 5);
		else if(buffer1 == "4")
			pc += (hex[j] * 4);
		else if(buffer1 == "3")
			pc += (hex[j] * 3);
		else if(buffer1 == "2")
			pc += (hex[j] * 2);
		else if(buffer1 == "1")
			pc += (hex[j] * 1);
		else if(buffer1 == "0")
			pc += (hex[j] * 0);
	}
	pc = pc / 4;//gets rid of the word align
	buffer1 = "";

	
	for(int c = 1; c < count; c++){//converts all of the commands from hex into binary
		for(int j = 0; j < 8; j++){
			buffer2 = cmds[c][j];
			if(buffer2 == "F" | buffer2 == "f" )
				buffer1 = buffer1 + "1111";
			else if(buffer2 == "E"  | buffer2 == "e")
				buffer1 = buffer1 + "1110";
			else if(buffer2 == "D" | buffer2 == "d")
				buffer1 = buffer1 + "1101";
			else if(buffer2 == "C" | buffer2 == "c")
				buffer1 = buffer1 + "1100";
			else if(buffer2 == "B" | buffer2 == "b" )
				buffer1 = buffer1 + "1011";
			else if(buffer2 == "A" | buffer2 == "a")
				buffer1 = buffer1 + "1010";
			else if(buffer2 == "9")
				buffer1 = buffer1 + "1001";
			else if(buffer2 == "8")
				buffer1 = buffer1 + "1000";
			else if(buffer2 == "7")
				buffer1 = buffer1 + "0111";
			else if(buffer2 == "6")
				buffer1 = buffer1 + "0110";
			else if(buffer2 == "5")
				buffer1 = buffer1 + "0101";
			else if(buffer2 == "4")
				buffer1 = buffer1 + "0100";
			else if(buffer2 == "3")
				buffer1 = buffer1 + "0011";
			else if(buffer2 == "2")
				buffer1 = buffer1 + "0010";
			else if(buffer2 == "1")
				buffer1 = buffer1 + "0001";
			else if(buffer2 == "0")
				buffer1 = buffer1 + "0000";
		}
		cmds[c] = buffer1;
		buffer1 = "";
	}
	
	for(int c = pc, j = 1; c < (pc + count - 1) && j < count; c++, j++){
			mem[c] = cmds[j];
	}
	memstart = pc;
	while(pc < (memstart + count - 1)){
		rdval = 0; rtval = 0; rsval = 0; immval = 0; addval = 0; shamtval = 0;
		opcode = mem[pc].substr(0, 6);
		if(opcode != "000010"){//if the opcode is anything but the jump opcode, obtain RS and RT
			rs = mem[pc].substr(6, 5);
			rt = mem[pc].substr(11, 5);
			if(opcode == "000000"){//if the opcode is any of the R opcodes, obtain RD, shamt, and funct
				rd = mem[pc].substr(16, 5);
				shamt = mem[pc].substr(21, 5);
				funct = mem[pc].substr(26, 6);
			}
			else if(opcode == "001000" | opcode == "000100" | opcode == "000101" | opcode == "001010")//if the opcode is any of the I opcodes, obtain the immediate
				immediate = mem[pc].substr(16, 16);
		}
		else if(opcode == "000010")//if the opcode is the jump opcode, obtain the address
			address = mem[pc].substr(6, 26);
		
		if(opcode == "000000" | opcode == "001000" | opcode == "000100" | opcode == "000101" | opcode == "001010"){//every function but jump.
			for(int i = 0, j = 4; i < 5, j >= 0; i++, j--){
				buffer1 = rs[i];
				buffer2 = rt[i];
				if(buffer1 == "1")//converts the binary values of rs to decimal
					rsval += immed[j];
				if(buffer2 == "1")//converts the binary value of rt to decimal
					rtval += immed[j];
			}
			if(opcode == "000000"){//takes care of all the r-functions
				for(int i = 0, j = 4; i < 5, j >= 0; i++, j--){//this bit of code converts the binary values of rd and shamt to decimal
					buffer1 = rd[i];
					buffer2 = shamt[i];
					if(buffer1 == "1")
						rdval += immed[j];
					if(buffer2 == "1")
						shamtval += immed[j];
				}
				if( funct == "100000"){//the add function
					R[rdval] = R[rsval] + R[rtval];
				}
				else if(funct == "100010"){//the subtract function
					R[rdval] = R[rsval] - R[rtval];
				}
				else if(funct == "101010"){//the slt function
					if(R[rsval] < R[rtval])
						R[rdval] = 1;
					else
						R[rdval] = 0;
				}
				else if(funct == "000000"){//the sll function
					R[rdval] = R[rtval] << shamtval;
				}
				else if(funct == "000010"){//the slr function
					R[rdval] = R[rtval] >> shamtval;
				}

			}
			else if(opcode == "001000" | opcode == "000100" | opcode == "000101" | opcode == "001010"){//all the i-functions
				for(int i = 0, j = 15; i < 16, j >= 0; i++, j--){
					buffer1 = immediate[i];
					if(buffer1 == "1")
						immval += immed[j];
				}
				if(opcode == "001000"){//the addi function
					R[rtval] = R[rsval] + immval;
				}
				else if(opcode == "000100"){//the beq function
					if(R[rsval] == R[rtval])
						pc += immval;
				}
				else if(opcode == "000101"){//the bnq function
					if(R[rsval] != R[rtval])
						pc += immval;
				}
				else if(opcode == "001010"){//the slti function
					if(R[rsval] < immval)
						R[rtval] = 1;
					else
						R[rtval] = 0;
				}
			}
		}
		else if(opcode == "000010"){
			for(int i = 0, j = 25; i < 26, j >= 0; i++, j--){
				buffer1 = address[i];
				if(buffer1 == "1")//converts the binary value of address to decimal
					addval += addr[j];
			}
			pc = addval - 1;//jumps to the intended address
		}
		else if(opcode == "111111"){
				if(mem[pc].substr(28, 4) == "1111"){//ends the program if FFFFFFFF is encountered
					cout << "Program terminated normally (0xFFFFFFFF)" << endl;
					break;
				}
				else if(mem[pc].substr(28, 4) == "0000"){//prints out the registers if FFFFFFF0 is encountered
					for(int c = 0; c < 32; c++){
						cout << c << ":";
						printf("%X ", R[c]);
					}
					printf("%X", pc*4);
					cout << endl;
					if( pc == (memstart + count - 2) ){//if FFFFFFF0 is the last command, print the dropped off bottom statement
						cout << "Program terminated (dropped off bottom)" << endl;
					}
				}
		}
		
		if((pc == (memstart + count - 2) && opcode != "111111") ){//looks to see if program count has been reached without the kill code
			cout << "Program terminated (dropped off bottom)" << endl;
		}
		pc++;
	}
	buffer1 = "";
}