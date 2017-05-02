#include <stdlib.h>
#include "CPU.h"
#include "Utils.h"
#include "Decoder/Decoder.h"

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
	IR_ARM ir;
	Decoder::Decode(ir, instr);

	if (Check_Condition(ir)) {
		ARM_Execute(ir);
	}
	// interrupts and prefetch aborts exist whether or not conditions are met 
}

bool CPU::Check_Condition(IR_ARM& ir) {
	switch (ir.cond) {
	case Conditions::EQ: return cpsr.flag_Z == 1;
	case Conditions::NE: return cpsr.flag_Z == 0;
	case Conditions::CS_HS: return cpsr.flag_C == 1;
	case Conditions::CC_LO: return cpsr.flag_C == 0;
	case Conditions::MI: return cpsr.flag_N == 1;
	case Conditions::PL: return cpsr.flag_N == 0;
	case Conditions::VS: return cpsr.flag_V == 1;
	case Conditions::VC: return cpsr.flag_V == 0;
	case Conditions::HI: return cpsr.flag_C == 1 && cpsr.flag_Z == 0;
	case Conditions::LS: return cpsr.flag_C == 0 || cpsr.flag_Z == 1;
	case Conditions::GE: return cpsr.flag_N == cpsr.flag_V;
	case Conditions::LT: return cpsr.flag_N != cpsr.flag_V;
	case Conditions::GT: return cpsr.flag_Z == 0 && cpsr.flag_N == cpsr.flag_V;
	case Conditions::LE: return cpsr.flag_Z == 1 && cpsr.flag_N != cpsr.flag_V;
	case Conditions::AL: return true;
	case Conditions::NV: throw "Unpredictable instructions are not emulated";
	}

	return false;
}

void CPU::ARM_Execute(IR_ARM& ir) {
	switch (ir.type) {
	case InstructionType::Data_Processing: Data_Processing(ir); break;
	case InstructionType::Branch: Branch(ir); break;
	case InstructionType::Multiply: throw std::string("Unimplemented opcode"); break;
	case InstructionType::Status_Regsiter_Access: throw std::string("Unimplemented opcode"); break;
	case InstructionType::Load_Store: throw std::string("Unimplemented opcode"); break;
	case InstructionType::Load_Store_Multiple: throw std::string("Unimplemented opcode"); break;
	case InstructionType::Semaphore: throw std::string("Unimplemented opcode"); break;
	case InstructionType::Exception_Generating: throw std::string("Unimplemented opcode"); break;
	case InstructionType::Coprocessor: throw std::string("Unimplemented opcode"); break;
	case InstructionType::Extensions: throw std::string("Unimplemented opcode"); break;
	}
}

inline void CPU::Multiply(IR_ARM& ir) {
	u32 Rm = ir.operand1;
	u32 Rs = ir.operand2;
	u32 Rn = ir.operand3;
	u32 Rd = ir.operand4;
	u32 RdLo = ir.operand3;
	u32 RdHi = ir.operand4;


	if (Rm == Regs::PC || Rs == Regs::PC || Rd == Regs::PC) {
		throw std::string("Unpredictable instructions are not emulated");
	}

	/*
	TODO: Check if there is support for early termination. From page 167:
		"If the multiplier implementation supports early termination, it must be implemented
		on the value of the <Rs> operand. The type of early termination used (signed or
		unsigned) is IMPLEMENTATION DEFINED."
	*/

	// C and V flags should be UNPREDICTABLE in armv4T but are unchanged in armv5 and after.

	switch (ir.instr) {
	case Instructions::MUL: MUL_Instr1(ir.s, Rd, getLo(static_cast<u64>(Rm) * static_cast<u64>(Rs)) ); break;
	case Instructions::MLA: MUL_Instr1(ir.s, Rd, getLo(static_cast<u64>(Rm) * static_cast<u64>(Rs)) + Rn); break;

	case Instructions::UMULL: {
		u64 result = static_cast<u64>(Rm) * static_cast<u64>(Rs);
		MUL_Instr2(ir.s, RdHi, RdLo, getHi(result), getLo(result));
		break;
	}

	case Instructions::UMLAL: {
		u64 result = static_cast<u64>(Rm) * static_cast<u64>(Rs);
		MUL_Instr2(ir.s, RdHi, RdLo, getHi(result) + gprs[RdHi] + CarryFrom(getLo(result), gprs[RdLo]) , getLo(result) + gprs[RdLo]);
		break;
	}

	case Instructions::SMULL: {
		s64 result = static_cast<s64>(Rm) * static_cast<s64>(Rs);
		MUL_Instr2(ir.s, RdHi, RdLo, getHi(result), getLo(result));
		break;
	}

	case Instructions::SMLAL: {
		s64 result = static_cast<s64>(Rm) * static_cast<s64>(Rs);
		MUL_Instr2(ir.s, RdHi, RdLo, getHi(result) + gprs[RdHi] + CarryFrom(getLo(result), gprs[RdLo]), getLo(result) + gprs[RdLo]);
		break;
	}

	}
}

inline void CPU::MUL_Instr1(bool S, unsigned Rd, u32 result) {
	gprs[Rd] = result;
	if(S){
		cpsr.flag_N = !!getBit(gprs[Rd], 31);
		cpsr.flag_Z = gprs[Rd] == 0;
	}
}

inline void CPU::MUL_Instr2(bool S, unsigned RdHi, unsigned RdLo, u32 resultHi, u32 resultLo) {
	gprs[RdHi] = resultHi;
	gprs[RdLo] = resultLo;
	if (S) {
		cpsr.flag_N = !!getBit(gprs[RdHi], 31);
		cpsr.flag_Z = (gprs[RdHi] == 0) & (gprs[RdLo] == 0);
	}
}

inline void CPU::Branch(IR_ARM& ir) {
	switch (ir.instr) {
	case Instructions::B: 
		gprs[Regs::PC] += SignExtend<s32>(ir.operand1 << 2, 26) + 8;
		break;
	
	case Instructions::BL:
		gprs[Regs::LR] = gprs[Regs::PC] + 4;
		gprs[Regs::PC] += SignExtend<s32>(ir.operand1 << 2, 26) + 8;
		break;

	case Instructions::BX:
		if ((gprs[ir.operand1] & 0b1) == 1) throw std::string("change to Thumb instructions is not supported");
		gprs[Regs::PC] += gprs[ir.operand1] & 0xFFFFFFFE;
		break;
	}
}

inline void CPU::Data_Processing(IR_ARM& ir) {
	u32 Rd = ir.operand1;
	u32 Rn = ir.operand2;
	u32 shifter_op;
	bool shifter_carry;
	std::tie(shifter_op, shifter_carry) = shifter_operand(ir.shifter_operand, false);

	switch (ir.instr) {
	case Instructions::AND: DP_Instr1(ir.s, Rd, gprs[Rn] & shifter_op, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, shifter_carry, cpsr.flag_V); break; //AND
	case Instructions::EOR: DP_Instr1(ir.s, Rd, gprs[Rn] ^ shifter_op, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, shifter_carry, cpsr.flag_V); break;
	case Instructions::SUB: DP_Instr1(ir.s, Rd, gprs[Rn] - shifter_op, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, !BorrowFromSub(gprs[Rn], shifter_op), OverflowFromSub(gprs[Rn], shifter_op)); break;
	case Instructions::RSB: DP_Instr1(ir.s, Rd, shifter_op - gprs[Rn], !!getBit(gprs[Rd], 31), gprs[Rd] == 0, !BorrowFromSub(shifter_op, gprs[Rn]), OverflowFromSub(shifter_op, gprs[Rn])); break;
	case Instructions::ADD: DP_Instr1(ir.s, Rd, gprs[Rn] + shifter_op, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, CarryFrom(gprs[Rn], shifter_op), OverflowFromAdd(gprs[Rn], shifter_op)); break;
	case Instructions::ADC: DP_Instr1(ir.s, Rd, gprs[Rn] + shifter_op + cpsr.flag_C, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, CarryFrom(gprs[Rn], shifter_op, cpsr.flag_C), OverflowFromAdd(gprs[Rn], shifter_op, cpsr.flag_C)); break;
	case Instructions::SBC: DP_Instr1(ir.s, Rd, gprs[Rn] - shifter_op - !cpsr.flag_C, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, !BorrowFromSub(gprs[Rn], shifter_op, !cpsr.flag_C), OverflowFromSub(gprs[Rn], shifter_op, !cpsr.flag_C)); break;
	case Instructions::RSC: DP_Instr1(ir.s, Rd, shifter_op - gprs[Rn] - !cpsr.flag_C, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, !BorrowFromSub(shifter_op, gprs[Rn], !cpsr.flag_C), OverflowFromSub(shifter_op, gprs[Rn], !cpsr.flag_C)); break;
	case Instructions::TST: DP_Instr2(gprs[Rn] & shifter_op, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, shifter_carry, cpsr.flag_V); break;
	case Instructions::TEQ: DP_Instr2(gprs[Rn] ^ shifter_op, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, shifter_carry, cpsr.flag_V); break;
	case Instructions::CMP: DP_Instr2(gprs[Rn] - shifter_op, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, !BorrowFromSub(gprs[Rn], shifter_op), OverflowFromSub(gprs[Rn], shifter_op)); break;
	case Instructions::CMN: DP_Instr2(gprs[Rn] + shifter_op, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, CarryFrom(gprs[Rn], shifter_op), OverflowFromAdd(gprs[Rn], shifter_op)); break;
	case Instructions::ORR: DP_Instr1(ir.s, Rd, gprs[Rn] | shifter_op, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, shifter_carry, cpsr.flag_V); break;
	case Instructions::MOV: DP_Instr1(ir.s, Rd, shifter_op, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, shifter_carry, cpsr.flag_V); break;
	case Instructions::BIC: DP_Instr1(ir.s, Rd, gprs[Rn] & ~shifter_op, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, shifter_carry, cpsr.flag_V); break;
	case Instructions::MVN: DP_Instr1(ir.s, Rd, ~shifter_op, !!getBit(gprs[Rd], 31), gprs[Rd] == 0, shifter_carry, cpsr.flag_V); break;
		//TODO verify negation for MVN and BIC
	}
}

inline void CPU::DP_Instr1(bool S, unsigned Rd, u32 result, bool N, bool Z, bool C, bool V) {
	gprs[Rd] = result;
	if (S && Rd == Regs::PC) {
		throw("no sprs in user/system mode, other mode not implemented yet");
	}
	else if (S) {
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

std::tuple<u32, bool> CPU::shifter_operand(Shifter_op& so, bool negatif) {

	u32 Rm = so.operand1;
	u32 Rs = so.operand2;
	u32 shift_imm = so.operand2;

	//TODO take care of PC as Rm, Rn, Rd, Rs
	if (Rm == Regs::PC || (Rs == Regs::PC && so.type != Shifter_type::Immediate && so.type != Shifter_type::LSL_imm && so.type != Shifter_type::LSR_imm && so.type != Shifter_type::ASR_imm && so.type != Shifter_type::ROR_imm && so.type != Shifter_type::RRX) ) {
		throw std::string("unimpletend: PC as Rm, Rn, Rd, Rs is either unpredictable or current instruction address + 8");
	}

	unsigned vRs7_0 = gprs[Rs] & 0xFF;
	unsigned vRs4_0 = Rs & 0b11111;

	switch (so.type) {
	case Shifter_type::Immediate: 
		if (so.operand2 == 0) return std::make_tuple(so.operand1, cpsr.flag_C);
		else return std::make_tuple(so.operand1, getBit(so.operand1, 31) == 1);
	
	case Shifter_type::Register: return std::make_tuple(gprs[Rm], cpsr.flag_C);
	
	case Shifter_type::LSL_imm: return std::make_tuple(gprs[Rm] << shift_imm, getBit(gprs[Rm], 32 - shift_imm) == 1);

	case Shifter_type::LSL_reg:
		if (vRs7_0 == 0) return std::make_tuple(gprs[Rm], cpsr.flag_C);
		else if (vRs7_0 < 32) return std::make_tuple(gprs[Rm] << vRs7_0, getBit(gprs[Rm], 32 - vRs7_0) == 1);
		else if (vRs7_0 == 32) return std::make_tuple(0, (gprs[Rm] & 0b1) == 1);
		else return std::make_tuple(0, false);

	case Shifter_type::LSR_imm: //>> is logical because gprs are unsigned
		if (shift_imm == 0)	return std::make_tuple(0, getBit(gprs[Rm], 31) == 1);
		else return std::make_tuple(gprs[Rm] >> shift_imm, getBit(gprs[Rm], shift_imm - 1) == 1);

	case Shifter_type::LSR_reg:
		if (vRs7_0 == 0) return std::make_tuple(gprs[Rm], cpsr.flag_C);
		else if (vRs7_0 < 32) return std::make_tuple(gprs[Rm] >> vRs7_0, getBit(gprs[Rm], vRs7_0 - 1) == 1);
		else if (vRs7_0 == 32) return std::make_tuple(0, getBit(gprs[Rm], 31) == 1);
		else return std::make_tuple(0, false);

	case Shifter_type::ASR_imm:
		if (shift_imm == 0)
			if (getBit(gprs[Rm], 31) == 0) return std::make_tuple(0, getBit(gprs[Rm], 31) == 1);
			else return std::make_tuple(0xFFFFFFFF, getBit(gprs[Rm], 31) == 1);
		else return std::make_tuple(static_cast<signed>(gprs[Rm]) >> shift_imm, getBit(gprs[Rm], shift_imm - 1) == 1);

	case Shifter_type::ASR_reg:
		if (vRs7_0 == 0) return std::make_tuple(gprs[Rm], cpsr.flag_C);
		else if (vRs7_0 < 32) return std::make_tuple(static_cast<signed>(gprs[Rm]) >> vRs7_0, getBit(gprs[Rm], vRs7_0 - 1) == 1);
		else if (getBit(gprs[Rm], 31) == 0) return std::make_tuple(0, getBit(gprs[Rm], 31) == 1);
		else return std::make_tuple(0xFFFFFFFF, getBit(gprs[Rm], 31) == 1);
	
	case Shifter_type::ROR_imm: return std::make_tuple(ror32(gprs[Rm], shift_imm), getBit(gprs[Rm], shift_imm - 1) == 1);

	case Shifter_type::ROR_reg:			
		if (vRs7_0 == 0) return std::make_tuple(gprs[Rm], cpsr.flag_C);
		else if (vRs4_0 == 0) return std::make_tuple(gprs[Rm], getBit(gprs[Rm], 31) == 1);
		else return std::make_tuple(ror32(gprs[Rm], vRs4_0), getBit(gprs[Rm], vRs4_0 - 1) == 1);

	case Shifter_type::RRX: return std::make_tuple((cpsr.flag_C << 31) | (gprs[Rm] >> 1), (gprs[Rm] & 0b1) == 1);

	default: throw "invalid shifter operand";
	}

}
