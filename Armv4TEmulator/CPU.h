#pragma once
#include <tuple>
#include "Common.h"
#include "Memory.h"
#include "Decoder\IR.h"

//TODO: make sure scope of the elements are appropriate

//shoud only be 5 bits long
enum struct CpuMode: u8 {
	User = 0b10000,
	FIQ = 0b10001,
	IRQ = 0b10010,
	Supervisor = 0b10011,
	Abort = 0b10111,
	Undefined = 0b11011,
	System = 0b11111
};


//TODO: Think about default values.
struct CPSR {
	bool flag_N, flag_Z, flag_C, flag_V; //bits 31 - 28
	u32 reserved; //bits 27 - 8
	bool flag_inter_I, flag_inter_F; //bits 7-6 
	bool flag_T; //bit 5
	CpuMode mode; //bits 4-0
};

class CPU {
public:
	//TODO: Take care of registers when in mode switch.

	/*
	ARM has 31 general-purpose 32-bit registers. At any one time, 16 of these registers are visible. The other
	registers are used to speed up exception processing. All the register specifiers in ARM instructions can
	address any of the 16 visible registers.
	-> which one is the 31th ? Can only see 30
	*/
	//use u32 or s32 ? -> mgba use s32
	u32 gprs[16];

	CPSR cpsr;
	CPSR spsr;
	Memory mem;

	CPU();
	void Step();
	bool Check_Condition(IR_ARM& ir);
	void ARM_Execute(IR_ARM& ir);
	void Data_Processing(IR_ARM& ir);
	void DP_Instr1(bool S, unsigned Rd, u32 result, bool N, bool Z, bool C, bool V);
	void DP_Instr2(u32 result, bool N, bool Z, bool C, bool V);
	std::tuple<u32, bool> CPU::shifter_operand(Shifter_op& so, bool negatif);
};
