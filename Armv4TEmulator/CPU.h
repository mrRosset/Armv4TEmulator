#pragma once
#include "Common.h"
#include "Memory.h"
#include <tuple>

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

namespace Regs {
	enum {
		/*
		Register R13 is normally used as a stack pointer and is also known as the SP. In the ARM instruction set,
		this is by convention only, as there are no defined instructions or other functionality which use R13 in a
		special-case manner. However, there are such instructions in the Thumb instruction set,
		*/
		SP = 13,
		LR = 14,
		/*
		Beware of all the special cases regarding R15. Also reading is a special thing. see page 38/39
		! see what implementation it is for STR and STM with R15 -> It's 12
		*/
		PC = 15
	};
}


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

	void Step();
	bool Check_Condition(u32 instr);
	void ARM_Execute(u32 instr);
	void Data_Processing(u32 instr);
	std::tuple<u32, bool> CPU::shifter_operand(u32 instr, unsigned I);
	void Add(unsigned S, unsigned Rd, unsigned Rn, u32 shifter_operand, bool shifter_carry);
};
