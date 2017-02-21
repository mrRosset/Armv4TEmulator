#include <stdlib.h>
#include "CPU.h"

/*
Code is microsoft-specific. Some functions for circluar functions are only in VC++
And the operator >> is used. This operator is logical/arithmetic depending on the implementation
In visual c++ it's arithmetic. https://msdn.microsoft.com/en-us/library/336xbhcz.aspx
*/

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
	case 0b1100: return cpsr.flag_Z == 0 && cpsr.flag_N == cpsr.flag_V;
	case 0b1101: return cpsr.flag_Z == 1 && cpsr.flag_N != cpsr.flag_V;
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
			Data_Processing(instr);
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
			Data_Processing(instr);
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

inline void CPU::Data_Processing(u32 instr) {
	unsigned opcode = (instr >> 21) & 0xF;
	unsigned I = (instr >> 25) & 0b1;
	unsigned S = (instr >> 20) & 0b1;
	unsigned Rn = (instr >> 16) & 0xF;
	unsigned Rd = (instr >> 12) & 0xF;
	u32 shifter_op;
	bool shifter_carry;
	std::tie(shifter_op, shifter_carry) = shifter_operand(instr, I);

	switch (opcode) {
	case 0b0000: And(S, Rd, Rn, shifter_op, shifter_carry); break;
	case 0b0001: Eor(S, Rd, Rn, shifter_op, shifter_carry); break;
	case 0b0010: Sub(S, Rd, Rn, shifter_op, shifter_carry); break;
	case 0b0011: Rsb(S, Rd, Rn, shifter_op, shifter_carry); break;
	case 0b0100: Add(S, Rd, Rn, shifter_op, shifter_carry); break;
	//case 0b0101: ADC; break;
	//case 0b0110: SBC; break;
	//case 0b0111: RSC; break;
	//case 0b1000: TST; break; <- no S, no Rd
	//case 0b1001: TEQ; break; <- no S, no Rd
	//case 0b1010: CMP; break; <- no S, no Rd
	//case 0b1011: CMN; break; <- no S, no Rd
	//case 0b1100: ORR; break;
	//case 0b1101: MOV; break; <- no Rn
	//case 0b1110: BIC; break;
	//case 0b1111: MVN; break; <- no Rn
	}
}

inline unsigned getBit(u32 v, int bit_number) {
	return ((v >> bit_number) & 0b1);
}

inline unsigned getBit(u32 v, unsigned bit_number) {
	return ((v >> bit_number) & 0b1);
}

//avoid implicit conversions
template <class T, class U>
unsigned getBit(T, U) = delete;

std::tuple<u32, bool> CPU::shifter_operand(u32 instr, unsigned I) {
	if (I) { //Immediate
		unsigned immed_8 = instr & 0xFF;
		unsigned rotate_imm = (instr >> 8) & 0xF;
		u32 result = _rotr(immed_8, rotate_imm * 2);
		if (rotate_imm == 0) 
			return std::make_tuple(result, cpsr.flag_C);
		else 
			return std::make_tuple(result, getBit(result, 31) == 1);
	}
	unsigned shift_imm = (instr >> 7) & 0b11111;
	unsigned Rs = (instr >> 8) & 0xF;
	unsigned Rm = instr & 0xF;
	//TODO take care of PC as Rm, Rn, Rd, Rs
	
	switch ((instr >> 4) & 0b111) {
	case 0b000: {
		if (shift_imm == 0)
			return std::make_tuple(gprs[Rm], cpsr.flag_C);
		else
			return std::make_tuple(gprs[Rm] << shift_imm, getBit(gprs[Rm], 32 - shift_imm) == 1);
		break;
	}
	case 0b001: {
		unsigned vRs7_0 = Rs & 0xFF;
		if (vRs7_0 == 0)
			return std::make_tuple(gprs[Rm], cpsr.flag_C);
		else if (vRs7_0 < 32)
			return std::make_tuple(gprs[Rm] << vRs7_0, getBit(gprs[Rm], 32 - vRs7_0) == 1);
		else if (vRs7_0 == 32)
			return std::make_tuple(0, (gprs[Rm] & 0b1) == 1);
		else 
			return std::make_tuple(0, false);
		break;
	}
	case 0b010: { //>> is logical because gprs are unsigned
		if (shift_imm == 0)
			return std::make_tuple(0, getBit(gprs[Rm], 31) == 1);
		else
			return std::make_tuple(gprs[Rm] >> shift_imm, getBit(gprs[Rm], shift_imm - 1) == 1);
		break;
	}
	case 0b011: {
		unsigned vRs7_0 = Rs & 0xFF;
		if (vRs7_0 == 0)
			return std::make_tuple(gprs[Rm], cpsr.flag_C);
		else if (vRs7_0 < 32)
			return std::make_tuple(gprs[Rm] >> vRs7_0, getBit(gprs[Rm], vRs7_0 - 1) == 1);
		else if (vRs7_0 == 32)
			return std::make_tuple(0, getBit(gprs[Rm], 31) == 1);
		else
			return std::make_tuple(0, false);
		break;
	}
	case 0b100: {
		if (shift_imm == 0)
			if (getBit(gprs[Rm], 31) == 0)
				return std::make_tuple(0, getBit(gprs[Rm], 31) == 1);
			else
				return std::make_tuple(0xFFFFFFFF, getBit(gprs[Rm], 31) == 1);
		else {
			signed vRm = gprs[Rm];
			return std::make_tuple(vRm >> shift_imm, getBit(gprs[Rm], shift_imm - 1) == 1);
		}
		break;
	}
	case 0b101: {
		unsigned vRs7_0 = Rs & 0xFF;
		signed int vRm = gprs[Rm];
		if (vRs7_0 == 0)
			return std::make_tuple(gprs[Rm], cpsr.flag_C);
		else if (vRs7_0 < 32)
			return std::make_tuple(gprs[Rm] >> vRs7_0, getBit(gprs[Rm], vRs7_0 - 1) == 1);
		else
			if (getBit(gprs[Rm], 31) == 0)
				return std::make_tuple(0, getBit(gprs[Rm], 31) == 1);
			else
				return std::make_tuple(0xFFFFFFFF, getBit(gprs[Rm], 31) == 1);
		break;
	}
	case 0b110: {
		if (shift_imm == 0) {
			return std::make_tuple((cpsr.flag_C << 31) & (gprs[Rm] >> 1), (gprs[Rm] & 0b1) == 1);
		}
		else
			return std::make_tuple(_rotr(gprs[Rm], shift_imm), getBit(gprs[Rm], shift_imm - 1) == 1);
		break;
	}
	case 0b111: {
		unsigned vRs7_0 = Rs & 0xFF;
		unsigned vRs4_0 = Rs & 0b11111;

		if (vRs7_0 == 0) {
			return std::make_tuple(gprs[Rm], cpsr.flag_C);
		}
		else if (vRs4_0 == 0) {
			return std::make_tuple(gprs[Rm], getBit(gprs[Rm], 31) == 1);
		}
		else {
			return std::make_tuple(_rotr(gprs[Rm], vRs4_0), getBit(gprs[Rm], vRs4_0 - 1) == 1);
		}
		break;
	}
	}
	
	throw "invalid shifter operand";
}

inline bool CarryFrom(u64 a, u64 b) {
	return (a + b) > UINT32_MAX;
}
inline bool CarryFrom(u64 a, u64 b, u64 c) {
	return (a + b + c) > UINT32_MAX;
}

//check no sign extension
inline bool OverflowFromAdd(s32 a, s32 b) {
	s32 r = a + b;
	return (a > 0 && b > 0 && r < 0) || (a < 0 && b < 0 && r > 0);
}
inline bool OverflowFromAdd(s32 a, s32 b, s32 c) {
	s32 r = a + b + c;
	return (a > 0 && b > 0 && r < 0) || (a < 0 && b < 0 && r > 0);
}

inline bool OverflowFromSub(s32 a, s32 b) {
	s32 r = a - b;
	return (a > 0 && b < 0 && r < 0) || (a < 0 && b > 0 && r > 0);
}

inline bool BorrowFromSub(u32 a, u32 b) {
	//To check
	return b > a;
}

inline void CPU::And(unsigned S, unsigned Rd, unsigned Rn, u32 shifter_operand, bool shifter_carry) {
	gprs[Rd] = gprs[Rn] & shifter_operand;
	if (S == 1 && Rd == Regs::PC) {
		throw("no sprs in user/system mode, other mode not implemented yet");
	}
	else if (S == 1) {
		cpsr.flag_N = getBit(gprs[Rd], 31) == 1;
		cpsr.flag_Z = gprs[Rd] == 0;
		cpsr.flag_C = shifter_carry;
	}
}

inline void CPU::Eor(unsigned S, unsigned Rd, unsigned Rn, u32 shifter_operand, bool shifter_carry) {
	gprs[Rd] = gprs[Rn] ^ shifter_operand;
	if (S == 1 && Rd == Regs::PC) {
		throw("no sprs in user/system mode, other mode not implemented yet");
	}
	else if (S == 1) {
		cpsr.flag_N = getBit(gprs[Rd], 31) == 1;
		cpsr.flag_Z = gprs[Rd] == 0;
		cpsr.flag_C = shifter_carry;
	}
}

inline void CPU::Sub(unsigned S, unsigned Rd, unsigned Rn, u32 shifter_operand, bool shifter_carry) {
	gprs[Rd] = gprs[Rn] - shifter_operand;
	if (S == 1 && Rd == Regs::PC) {
		throw("no sprs in user/system mode, other mode not implemented yet");
	}
	else if (S == 1) {
		cpsr.flag_N = getBit(gprs[Rd], 31) == 1;
		cpsr.flag_Z = gprs[Rd] == 0;
		cpsr.flag_C = !BorrowFromSub(gprs[Rn], shifter_operand);
		cpsr.flag_V = OverflowFromSub(gprs[Rn], shifter_operand);
	}
}

inline void CPU::Rsb(unsigned S, unsigned Rd, unsigned Rn, u32 shifter_operand, bool shifter_carry) {
	gprs[Rd] = shifter_operand - gprs[Rn];
	if (S == 1 && Rd == Regs::PC) {
		throw("no sprs in user/system mode, other mode not implemented yet");
	}
	else if (S == 1) {
		cpsr.flag_N = getBit(gprs[Rd], 31) == 1;
		cpsr.flag_Z = gprs[Rd] == 0;
		cpsr.flag_C = !BorrowFromSub(shifter_operand, gprs[Rn]);
		cpsr.flag_V = OverflowFromSub(shifter_operand, gprs[Rn]);
	}
}

inline void CPU::Add(unsigned S, unsigned Rd, unsigned Rn, u32 shifter_operand, bool shifter_carry) {
	gprs[Rd] = gprs[Rn] + shifter_operand;
	if (S == 1 && Rd == Regs::PC) {
		throw("no sprs in user/system mode, other mode not implemented yet");
	} 
	else if (S == 1) {
		cpsr.flag_N = getBit(gprs[Rd], 31) == 1;
		cpsr.flag_Z = gprs[Rd] == 0;
		cpsr.flag_C = CarryFrom(gprs[Rn], shifter_operand);
		cpsr.flag_V = OverflowFromAdd(gprs[Rn], shifter_operand);
	}
}