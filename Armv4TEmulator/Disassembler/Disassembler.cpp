#include <cstdlib>
#include "Disassembler.h"
#include "../Utils.h"



std::string Disassembler::Disassemble(IR_ARM & ir) {
	std::string s = ir.s ? "s" : "";
	
	switch (ir.instr) {

	//Data Processing Instructions
	case Instructions::TST: return "tst" + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand2) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::TEQ: return "teq" + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand2) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::CMP: return "cmp" + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand2) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::CMN: return "cmn" + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand2) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::MOV: return "mov" + s + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::MVN: return "mvn" + s + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::AND: return "and" + s + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::EOR: return "eor" + s + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::SUB: return "sub" + s + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::RSB: return "rsb" + s + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::ADD: return "add" + s + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::ADC: return "adc" + s + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::SBC: return "sbc" + s + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::RSC: return "rsc" + s + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::ORR: return "orr" + s + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::BIC: return "bic" + s + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);

	//Branch Instructions
	case Instructions::B: return "b" + Disassemble_Cond(ir) + " " + Disassemble_Branch_Offset(ir.operand1);
	case Instructions::BL: return "bl" + Disassemble_Cond(ir) + " " + Disassemble_Branch_Offset(ir.operand1) ;
	case Instructions::BX: return "bx" + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand1);

	//Multiply Instructions
	case Instructions::MUL: return "mul" + s + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand4) + ", " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2); break;
	case Instructions::MLA: return "mla" + s + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand4) + ", " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2) + ", " + Disassemble_Reg(ir.operand3); break;
	case Instructions::UMULL: return "umull" + s + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand3) + ", " + Disassemble_Reg(ir.operand4) + ", " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2); break;
	case Instructions::UMLAL: return "umlal" + s + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand3) + ", " + Disassemble_Reg(ir.operand4) + ", " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2); break;
	case Instructions::SMULL: return "smull" + s + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand3) + ", " + Disassemble_Reg(ir.operand4) + ", " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2); break;
	case Instructions::SMLAL: return "smlal" + s + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand3) + ", " + Disassemble_Reg(ir.operand4) + ", " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2); break;

	//Status Register Access Instructions
	case Instructions::MRS: return "mrs" + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand2) + ", " + Disassemble_PSR(ir.operand1); break;
	case Instructions::MSR: return "msr" + Disassemble_Cond(ir) + " " + Disassemble_PSR(ir.operand1) + "_" + Disassemble_Fields(ir.operand2) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand); break;
	
	//Load/Store unsigned byte/word
	case Instructions::LDR: return "ldr" + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_LS_Shifter_Operand(ir); break;
	case Instructions::STR: return "str" + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_LS_Shifter_Operand(ir); break;
	case Instructions::LDRT: return "ldrt" + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_LS_Shifter_Operand(ir); break;
	case Instructions::STRT: return "strt" + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_LS_Shifter_Operand(ir); break;
	case Instructions::LDRB: return "ldrb" + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_LS_Shifter_Operand(ir); break;
	case Instructions::STRB: return "strb" + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_LS_Shifter_Operand(ir); break;
	case Instructions::LDRBT: return "ldrbt" + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_LS_Shifter_Operand(ir); break;
	case Instructions::STRBT: return "strbt" + Disassemble_Cond(ir) + " " + Disassemble_Reg(ir.operand1) + ", *" + Disassemble_LS_Shifter_Operand(ir); break;
	}
	return std::string();
}

std::string Disassembler::Disassemble_Reg(u32 reg) {
	switch (reg) {
	case 13: return "sp";
	case 14: return "lr";
	case 15: return "pc";
	default: return "r" + std::to_string(reg);
	}
}

std::string Disassembler::Disassemble_Cond(IR_ARM & ir) {
	switch (ir.cond) {
	case Conditions::EQ: return "eq";
	case Conditions::NE: return "ne";
	case Conditions::CS_HS: return "cs";
	case Conditions::CC_LO: return "cc";
	case Conditions::MI: return "mi";
	case Conditions::PL: return "pl";
	case Conditions::VS: return "vs";
	case Conditions::VC: return "vc";
	case Conditions::HI: return "hi";
	case Conditions::LS: return "ls";
	case Conditions::GE: return "ge";
	case Conditions::LT: return "lt";
	case Conditions::GT: return "gt";
	case Conditions::LE: return "le";
	case Conditions::AL: return "";
	case Conditions::NV: return "nv";
	}
}

std::string Disassembler::Disassemble_Shifter_Operand(Shifter_op& so, bool negatif) {
	std::string negation = negatif ? "-" : "";
	switch (so.type) {
	case Immediate: return "#" + negation + std::to_string(so.operand1);
	case Register: return Disassemble_Reg(so.operand1);
	case LSL_imm: return Disassemble_Reg(so.operand1) + ", lsl #" + negation + std::to_string(so.operand2);
	case LSL_reg: return Disassemble_Reg(so.operand1) + ", lsl " + Disassemble_Reg(so.operand2);
	case LSR_imm: return Disassemble_Reg(so.operand1) + ", lsr #" + negation + std::to_string(so.operand2);
	case LSR_reg: return Disassemble_Reg(so.operand1) + ", lsr " + Disassemble_Reg(so.operand2);
	case ASR_imm: return Disassemble_Reg(so.operand1) + ", asr #" + negation + std::to_string(so.operand2);
	case ASR_reg: return Disassemble_Reg(so.operand1) + ", asr " + Disassemble_Reg(so.operand2);
	case ROR_imm: return Disassemble_Reg(so.operand1) + ", ror #" + negation + std::to_string(so.operand2);
	case ROR_reg: return Disassemble_Reg(so.operand1) + ", ror " + Disassemble_Reg(so.operand2);
	case RRX: return Disassemble_Reg(so.operand1) + ", rrx";
	}
}

std::string Disassembler::Disassemble_Shifter_Operand(Shifter_op& so) {
	return Disassemble_Shifter_Operand(so, false);
}

std::string Disassembler::Disassemble_Branch_Offset(u32 operand) {
	u32 offset = SignExtend<s32>(operand << 2, 26) + 8;
	std::string sign = offset >= 0 ? "+" : "-";
	return sign + "#" + std::to_string(offset);
}

std::string Disassembler::Disassemble_PSR(u32 R) {
	return R == 1 ? "SPSR" : "CPSR";
}

std::string Disassembler::Disassemble_Fields(u32 fields) {
	std::string r = "";
	r += getBit(fields, 3) == 1 ? "f" : "";
	r += getBit(fields, 2) == 1 ? "s" : "";
	r += getBit(fields, 1) == 1 ? "x" : "";
	r += getBit(fields, 0) == 1 ? "c" : "";
	return r;
}

std::string Disassembler::Disassemble_LS_Shifter_Operand(IR_ARM& ir) {
	bool P = (ir.operand3 & 0b1000) >> 3 == 1;
	bool U = (ir.operand3 & 0b0100) >> 2== 1;
	bool W = (ir.operand3 & 0b0001) == 1;

	auto preindexed = (ir.operand3 & 0b1001) == 0b1001 ? "!" : "";
	if (!P) {
		return "[" + Disassemble_Reg(ir.operand2) + "], " + Disassemble_Shifter_Operand(ir.shifter_operand, !U);
	}
	else if (P && W) {
		return "[" + Disassemble_Reg(ir.operand2) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand, !U) + "]!";
	}
	else {
		return "[" + Disassemble_Reg(ir.operand2) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand, !U) + "]";
	}
}