#include "Disassembler.h"
#include "../Utils.h"



std::string Disassmbler::Disassemble(IR_ARM & ir) {
	std::string s = ir.operand1 == 1 ? "s" : "";
	
	switch (ir.instr) {

	//Data Processing instructions
	case Instructions::AND: return "and" + s + Disassemble_Condition(ir) + " r" + std::to_string(ir.operand2) + ", r" + std::to_string(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::EOR: return "eor" + s + Disassemble_Condition(ir) + " r" + std::to_string(ir.operand2) + ", r" + std::to_string(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::SUB: return "sub" + s + Disassemble_Condition(ir) + " r" + std::to_string(ir.operand2) + ", r" + std::to_string(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::RSB: return "rsb" + s + Disassemble_Condition(ir) + " r" + std::to_string(ir.operand2) + ", r" + std::to_string(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::ADD: return "add" + s + Disassemble_Condition(ir) + " r" + std::to_string(ir.operand2) + ", r" + std::to_string(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::ADC: return "adc" + s + Disassemble_Condition(ir) + " r" + std::to_string(ir.operand2) + ", r" + std::to_string(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::SBC: return "sbc" + s + Disassemble_Condition(ir) + " r" + std::to_string(ir.operand2) + ", r" + std::to_string(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::RSC: return "rsc" + s + Disassemble_Condition(ir) + " r" + std::to_string(ir.operand2) + ", r" + std::to_string(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::TST: return "tst" + s + Disassemble_Condition(ir) + " r" + std::to_string(ir.operand2) + ", r" + std::to_string(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::TEQ: return "teq" + s + Disassemble_Condition(ir) + " r" + std::to_string(ir.operand2) + ", r" + std::to_string(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::CMP: return "cmp" + s + Disassemble_Condition(ir) + " r" + std::to_string(ir.operand2) + ", r" + std::to_string(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::CMN: return "cmn" + s + Disassemble_Condition(ir) + " r" + std::to_string(ir.operand2) + ", r" + std::to_string(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::ORR: return "orr" + s + Disassemble_Condition(ir) + " r" + std::to_string(ir.operand2) + ", r" + std::to_string(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::MOV: return "mov" + s + Disassemble_Condition(ir) + " r" + std::to_string(ir.operand2) + ", r" + std::to_string(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::MVN: return "mvn" + s + Disassemble_Condition(ir) + " r" + std::to_string(ir.operand2) + ", r" + std::to_string(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);
	case Instructions::BIC: return "bic" + s + Disassemble_Condition(ir) + " r" + std::to_string(ir.operand2) + ", r" + std::to_string(ir.operand3) + ", " + Disassemble_Shifter_Operand(ir.shifter_operand);

	}
	return std::string();
}

std::string Disassmbler::Disassemble_Condition(IR_ARM & ir) {
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

std::string Disassmbler::Disassemble_Shifter_Operand(Shifter_op& so) {
	switch (so.type) {
	case Immediate: return "#" + std::to_string(ror32(so.operand2, so.operand1 * 2));
	case Register: return "r" + std::to_string(so.operand1);
	case LSL_imm: return "r" + std::to_string(so.operand1) + ", LSL #" + std::to_string(so.operand2);
	case LSL_reg: return "r" + std::to_string(so.operand1) + ", LSL r" + std::to_string(so.operand2);
	case LSR_imm: return "r" + std::to_string(so.operand1) + ", LSR #" + std::to_string(so.operand2);
	case LSR_reg: return "r" + std::to_string(so.operand1) + ", LSR r" + std::to_string(so.operand2);
	case ASR_imm: return "r" + std::to_string(so.operand1) + ", ASR #" + std::to_string(so.operand2);
	case ASR_reg: return "r" + std::to_string(so.operand1) + ", ASR r" + std::to_string(so.operand2);
	case ROR_imm: return "r" + std::to_string(so.operand1) + ", ROR #" + std::to_string(so.operand2);
	case ROR_reg: return "r" + std::to_string(so.operand1) + ", ROR r" + std::to_string(so.operand2);
	case RRX: return "r" + std::to_string(so.operand1) + ", RRX";
	}
}
