#include <stdlib.h>
#include "CPU.h"
#include "Utils.h"

/*
Code in this file is compiler-specific.
the operator >> is used. This operator is can be logical or arithmetic
In VC++ it's arithmetic. https://msdn.microsoft.com/en-us/library/336xbhcz.aspx
Not tested in any other compiler, 
*/



CPU::CPU() : gprs{} {
	//TODO: Seriously think about initialization values
	cpsr.flag_N = cpsr.flag_Z = cpsr.flag_C = cpsr.flag_V = cpsr.flag_inter_I = cpsr.flag_inter_F = cpsr.flag_T = false;
	cpsr.reserved = 0;
	cpsr.mode = CpuMode::User;
}

void CPU::Step() {
	u32 instr = mem.read32(gprs[Regs::PC]);
	if (Check_Condition(instr)) {
		ARM_Execute(instr);
	}
	// interrupts and prefetch aborts exist whether or not conditions are met 
}

bool CPU::Check_Condition(u32 instr) {
	switch ((instr >> 28) & 0b1111) {
	case 0b0000: return cpsr.flag_Z == 1;
	case 0b0001: return cpsr.flag_Z == 0;
	case 0b0010: return cpsr.flag_C == 1;
	case 0b0011: return cpsr.flag_C == 0;
	case 0b0100: return cpsr.flag_N == 1;
	case 0b0101: return cpsr.flag_N == 0;
	case 0b0110: return cpsr.flag_V == 1;
	case 0b0111: return cpsr.flag_V == 0;
	case 0b1000: return cpsr.flag_C == 1 && cpsr.flag_Z == 0;
	case 0b1001: return cpsr.flag_C == 0 || cpsr.flag_Z == 1;
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
	case 0b010: // Load / store immediate offset
	case 0b011: // Load / store register offset
	case 0b100: // Load/store multiple
	case 0b101: // Branch and branch with link
	case 0b110: // Coprocessor load/store and double register transfers[<- does it exist without the DSP extension ?
	case 0b111: // Coprocessor + Software interrupts
	default: throw std::string("Unimplemented opcode");
	}
}

inline bool CarryFrom(u64 a, u64 b) {
	return (a + b) > UINT32_MAX;
}
inline bool CarryFrom(u64 a, u64 b, u64 c) {
	return (a + b + c) > UINT32_MAX;
}

//TODO: check no sign extension
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
inline bool OverflowFromSub(s32 a, s32 b, s32 c) {
	s32 r = a - b - c;
	return (a > 0 && b < 0 && r < 0) || (a < 0 && b > 0 && r > 0);
}

inline bool BorrowFromSub(u32 a, u32 b) {
	//To check
	return b > a;
}
inline bool BorrowFromSub(u32 a, u32 b, u32 c) {
	//To check
	return b + c > a;
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
	case 0b0000: DP_Instr1(S, Rd, gprs[Rn] & shifter_op, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, shifter_carry, cpsr.flag_V); break; //AND
	case 0b0001: DP_Instr1(S, Rd, gprs[Rn] ^ shifter_op, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, shifter_carry, cpsr.flag_V); break; //EOR
	case 0b0010: DP_Instr1(S, Rd, gprs[Rn] - shifter_op, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, !BorrowFromSub(gprs[Rn], shifter_op), OverflowFromSub(gprs[Rn], shifter_op)); break; //SUB
	case 0b0011: DP_Instr1(S, Rd, shifter_op - gprs[Rn], !!getBit(gprs[Rd], 31), gprs[Rd] == 0, !BorrowFromSub(shifter_op, gprs[Rn]), OverflowFromSub(shifter_op, gprs[Rn])); break; //RSB
	case 0b0100: DP_Instr1(S, Rd, gprs[Rn] + shifter_op, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, CarryFrom(gprs[Rn], shifter_op), OverflowFromAdd(gprs[Rn], shifter_op)); break; //ADD
	case 0b0101: DP_Instr1(S, Rd, gprs[Rn] + shifter_op + cpsr.flag_C, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, CarryFrom(gprs[Rn], shifter_op, cpsr.flag_C), OverflowFromAdd(gprs[Rn], shifter_op, cpsr.flag_C)); break; //ADC
	case 0b0110: DP_Instr1(S, Rd, gprs[Rn] - shifter_op - !cpsr.flag_C, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, !BorrowFromSub(gprs[Rn], shifter_op, !cpsr.flag_C), OverflowFromSub(gprs[Rn], shifter_op, !cpsr.flag_C)); break; //SBC
	case 0b0111: DP_Instr1(S, Rd, shifter_op - gprs[Rn] - !cpsr.flag_C, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, !BorrowFromSub(shifter_op, gprs[Rn], !cpsr.flag_C), OverflowFromSub(shifter_op, gprs[Rn], !cpsr.flag_C)); break; //RSC
	case 0b1000: DP_Instr2(gprs[Rn] & shifter_op, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, shifter_carry, cpsr.flag_V); break; //TST
	case 0b1001: DP_Instr2(gprs[Rn] ^ shifter_op, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, shifter_carry, cpsr.flag_V); break; //TEQ 
	case 0b1010: DP_Instr2(gprs[Rn] - shifter_op, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, !BorrowFromSub(gprs[Rn], shifter_op), OverflowFromSub(gprs[Rn], shifter_op)); break; //CMP
	case 0b1011: DP_Instr2(gprs[Rn] + shifter_op, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, CarryFrom(gprs[Rn], shifter_op), OverflowFromAdd(gprs[Rn], shifter_op)); break; //CMN
	case 0b1100: DP_Instr1(S, Rd, gprs[Rn] | shifter_op, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, shifter_carry, cpsr.flag_V); break; //ORR
	case 0b1101: DP_Instr1(S, Rd, shifter_op, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, shifter_carry, cpsr.flag_V); break; //MOV
	case 0b1110: DP_Instr1(S, Rd, gprs[Rn] & ~shifter_op, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, shifter_carry, cpsr.flag_V); break; //BIC
	case 0b1111: DP_Instr1(S, Rd, ~shifter_op, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, shifter_carry, cpsr.flag_V); break; //MVN
	//TODO verify negation for MVN and BIC
	}
}

inline void CPU::DP_Instr1(unsigned S, unsigned Rd, u32 result, bool N, bool Z, bool C, bool V) {
	gprs[Rd] = result;
	if (S == 1 && Rd == Regs::PC) {
		throw("no sprs in user/system mode, other mode not implemented yet");
	}
	else if (S == 1) {
		cpsr.flag_N = N;
		cpsr.flag_Z = Z;
		cpsr.flag_C = C;
		cpsr.flag_V = V;
	}
}

inline void CPU::DP_Instr2(u32 result, bool N, bool Z, bool C, bool V) {
	//TODO: What is alu_out ? what's supposed to happens to the result ?
	u32 alu_out = result;
	cpsr.flag_N = N;
	cpsr.flag_Z = Z;
	cpsr.flag_C = C;
	cpsr.flag_V = V;
}

std::tuple<u32, bool> CPU::shifter_operand(u32 instr, unsigned I) {
	if (I) { //Immediate
		unsigned immed_8 = instr & 0xFF;
		unsigned rotate_imm = (instr >> 8) & 0xF;
		u32 result = ror32(immed_8, rotate_imm * 2);
		if (rotate_imm == 0)
			return std::make_tuple(result, cpsr.flag_C);
		else
			return std::make_tuple(result, getBit(result, 31) == 1);
	}
	unsigned shift_imm = (instr >> 7) & 0b11111;
	unsigned Rs = (instr >> 8) & 0xF;
	unsigned Rm = instr & 0xF;
	//TODO take care of PC as Rm, Rn, Rd, Rs
	if (Rm == Regs::PC || Rs == Regs::PC) {
		throw std::string("unimpletend: PC as Rm, Rn, Rd, Rs is either unpredictable or current instruction address + 8");
	}

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
			return std::make_tuple(ror32(gprs[Rm], shift_imm), getBit(gprs[Rm], shift_imm - 1) == 1);
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
			return std::make_tuple(ror32(gprs[Rm], vRs4_0), getBit(gprs[Rm], vRs4_0 - 1) == 1);
		}
		break;
	}
	}

	throw "invalid shifter operand";
}
