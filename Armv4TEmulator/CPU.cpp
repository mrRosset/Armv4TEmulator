#include "CPU.h"



void CPU::Step() {
	u32 instr = mem.read32(gprs[Regs::PC]);
	if (Check_Condition(instr)) {
		ARM_Execute(instr);
	}
	// interrupts and prefetch aborts exist whether or not conditions are met 
}

bool CPU::Check_Condition(u32 instr) {
	switch ((instr >> 28) | 0b1111) {
	case 0b0000: return cpsr.flag_Z == 1;
	case 0b0001: return cpsr.flag_Z == 0;
	case 0b0010: return cpsr.flag_C == 1;
	case 0b0011: return cpsr.flag_C == 0;
	case 0b0100: return cpsr.flag_N == 1;
	case 0b0101: return cpsr.flag_N == 0;
	case 0b0110: return cpsr.flag_V == 1;
	case 0b0111: return cpsr.flag_V == 0;
	case 0b1000: return cpsr.flag_C == 1 && cpsr.flag_Z == 0;
	case 0b1001: return cpsr.flag_C == 0 && cpsr.flag_Z == 1;
	case 0b1010: return cpsr.flag_N == cpsr.flag_V;
	case 0b1011: return cpsr.flag_N != cpsr.flag_V;
	case 0b1100: return cpsr.flag_Z == 0 & cpsr.flag_N == cpsr.flag_V;
	case 0b1101: return cpsr.flag_Z == 1 & cpsr.flag_N != cpsr.flag_V;
	case 0b1110: return true;
	case 0b1111: throw "Unpredictable instructions are not emulated";
	}

	return false;
}


void CPU::ARM_Execute(u32 instr) {
	unsigned bits28_25 = (instr >> 25) & 0b111;

	switch (bits28_25) {
	case 0b000:  // Data processing + Miscellaneous
	{
		unsigned bit4 = (instr >> 4) & 0b1;
		unsigned bit7= (instr >> 7) & 0b1;
		unsigned bit20 = (instr >> 20) & 0b1;
		unsigned bit24_23 = (instr >> 23) & 0b11;
		
		if (bit24_23 == 0b10 && bit20 == 1) {
			throw "3-3 not done";
		}
		else if (bit7 == 1 && bit4 == 1) {
			throw "3-2 not done";
		}
		else {
			throw "data processing not done";
		}
		break;
	}
	case 0b001: ; // Data processing immediate + Move immediate to status register
	{
		unsigned bit21_20 = (instr >> 20) & 0b11;
		unsigned bit24_23 = (instr >> 23) & 0b11;
		if (bit24_23 == 0b10 && bit21_20 == 0b00) {
			throw "Undefined instruction are not emulated";
		}
		else if (bit24_23 == 0b10 && bit21_20 == 10) {
			throw "Move immediate to status register not done";
		}
		else {
			throw "data processing not done";
		}
		
		break;
	}
	case 0b010: break; // Load / store immediate offset
	case 0b011: break; // Load / store register offset
	case 0b100: break; // Load/store multiple
	case 0b101: break; // Branch and branch with link
	case 0b110: break; // Coprocessor load/store and double register transfers[<- does it exist without the DSP extension ?
	case 0b111: break; // Coprocessor + Software interrupts
	}
}