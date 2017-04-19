#include <cstdlib>
#include "Disassembler.h"
#include "../Utils.h"



std::string Disassembler::Disassemble(IR_ARM & ir) {
	std::string s = ir.operand1 == 1 ? "s" : "";
	
	switch (ir.instr) {

	//Data Processing instructions
	case Instructions::TST: return "tst" + Disassemble_Condition(ir) + " " + Disassemble_register(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::TEQ: return "teq" + Disassemble_Condition(ir) + " " + Disassemble_register(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::CMP: return "cmp" + Disassemble_Condition(ir) + " " + Disassemble_register(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::CMN: return "cmn" + Disassemble_Condition(ir) + " " + Disassemble_register(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::MOV: return "mov" + s + Disassemble_Condition(ir) + " " + Disassemble_register(ir.operand2) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::MVN: return "mvn" + s + Disassemble_Condition(ir) + " " + Disassemble_register(ir.operand2) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::AND: return "and" + s + Disassemble_Condition(ir) + " " + Disassemble_register(ir.operand2) + ", " + Disassemble_register(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::EOR: return "eor" + s + Disassemble_Condition(ir) + " " + Disassemble_register(ir.operand2) + ", " + Disassemble_register(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::SUB: return "sub" + s + Disassemble_Condition(ir) + " " + Disassemble_register(ir.operand2) + ", " + Disassemble_register(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::RSB: return "rsb" + s + Disassemble_Condition(ir) + " " + Disassemble_register(ir.operand2) + ", " + Disassemble_register(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::ADD: return "add" + s + Disassemble_Condition(ir) + " " + Disassemble_register(ir.operand2) + ", " + Disassemble_register(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::ADC: return "adc" + s + Disassemble_Condition(ir) + " " + Disassemble_register(ir.operand2) + ", " + Disassemble_register(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::SBC: return "sbc" + s + Disassemble_Condition(ir) + " " + Disassemble_register(ir.operand2) + ", " + Disassemble_register(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::RSC: return "rsc" + s + Disassemble_Condition(ir) + " " + Disassemble_register(ir.operand2) + ", " + Disassemble_register(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::ORR: return "orr" + s + Disassemble_Condition(ir) + " " + Disassemble_register(ir.operand2) + ", " + Disassemble_register(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::BIC: return "bic" + s + Disassemble_Condition(ir) + " " + Disassemble_register(ir.operand2) + ", " + Disassemble_register(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);

	//Branch Instructions
	case Instructions::B: return "b" + Disassemble_Condition(ir) + " " + Disassemble_Branch_Offset(ir.operand1);
	case Instructions::BL: return "bl" + Disassemble_Condition(ir) + " " + Disassemble_Branch_Offset(ir.operand1) ;
	case Instructions::BX: return "bx" + Disassemble_Condition(ir) + " " + Disassemble_register(ir.operand1);

	}
	return std::string();
}

std::string Disassembler::Disassemble_register(u32 reg) {
	switch (reg) {
	case 13: return "sp";
	case 14: return "lr";
	case 15: return "pc";
	default: return "r" + std::to_string(reg);
	}
}

std::string Disassembler::Disassemble_Condition(IR_ARM & ir) {
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

std::string Disassembler::Disassemble_Shifter_Operand(Shifter_op& so) {
	switch (so.type) {
	case Immediate: return "#" + std::to_string(ror32(so.operand2, so.operand1 * 2));
	case Register: return Disassemble_register(so.operand1);
	case LSL_imm: return Disassemble_register(so.operand1) + ", lsl #" + std::to_string(so.operand2);
	case LSL_reg: return Disassemble_register(so.operand1) + ", lsl " + Disassemble_register(so.operand2);
	case LSR_imm: return Disassemble_register(so.operand1) + ", lsr #" + std::to_string(so.operand2);
	case LSR_reg: return Disassemble_register(so.operand1) + ", lsr " + Disassemble_register(so.operand2);
	case ASR_imm: return Disassemble_register(so.operand1) + ", asr #" + std::to_string(so.operand2);
	case ASR_reg: return Disassemble_register(so.operand1) + ", asr " + Disassemble_register(so.operand2);
	case ROR_imm: return Disassemble_register(so.operand1) + ", ror #" + std::to_string(so.operand2);
	case ROR_reg: return Disassemble_register(so.operand1) + ", ror " + Disassemble_register(so.operand2);
	case RRX: return Disassemble_register(so.operand1) + ", rrx";
	}
}

std::string Disassembler::Disassemble_Branch_Offset(u32 operand) {
	u32 offset = SignExtend<s32>(operand << 2, 26) + 8;
	std::string sign = offset >= 0 ? "+" : "-";
	return sign + "#" + std::to_string(offset);
}