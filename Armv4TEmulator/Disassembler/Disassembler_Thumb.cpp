#include "Disassembler.h"
#include "../Utils.h"


std::string Disassembler::Disassemble(IR_Thumb& ir) {
	
	switch (ir.instr) {

	//Branch Instructions
	case TInstructions::B_cond:  return "b" + Disassemble_Cond(ir.cond) + " " + Disassemble_Branch_Signed_Offset(SignExtend<s32>(ir.operand1 << 1, 9) + 4);
	case TInstructions::B_imm:   return "b " + Disassemble_Branch_Signed_Offset(SignExtend<s32>(ir.operand1 << 1, 12) + 4);
	case TInstructions::BL_high: return "bl high " + Disassemble_Branch_Signed_Offset((SignExtend<s32>(ir.operand1, 11) << 12) + 4);
	case TInstructions::BL:      return "bl #" + std::to_string(ir.operand1 << 1);
	case TInstructions::BX:      return "bx " + Disassemble_Reg(ir.operand1);
	
	//Data Processing Instructions
	case TInstructions::ADD_reg: return "add " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2) + ", " + Disassemble_Reg(ir.operand3);
	case TInstructions::SUB_reg: return "sub " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2) + ", " + Disassemble_Reg(ir.operand3);
	case TInstructions::ADD_imm: return "add " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2) + ", #" + std::to_string(ir.operand3);
	case TInstructions::SUB_imm: return "sub " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2) + ", #" + std::to_string(ir.operand3);

	case TInstructions::LSL_imm: return "lsl " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2) + ", #" + std::to_string(ir.operand3);
	case TInstructions::LSR_imm: return "lsr " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2) + ", #" + std::to_string(ir.operand3);
	case TInstructions::ASR_imm: return "asr " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2) + ", #" + std::to_string(ir.operand3);
	
	case     TInstructions::MOV_imm: return "mov " + Disassemble_Reg(ir.operand2) + ", #" + std::to_string(ir.operand1);
	case     TInstructions::CMP_imm: return "cmp " + Disassemble_Reg(ir.operand2) + ", #" + std::to_string(ir.operand1);
	case TInstructions::ADD_lar_imm: return "add " + Disassemble_Reg(ir.operand2) + ", #" + std::to_string(ir.operand1);
	case TInstructions::SUB_lar_imm: return "sub " + Disassemble_Reg(ir.operand2) + ", #" + std::to_string(ir.operand1);

	case     TInstructions::AND: return "and " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2);
	case     TInstructions::EOR: return "eor " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2);
	case TInstructions::LSL_reg: return "lsl " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2);
	case TInstructions::LSR_reg: return "lsr " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2);
	case TInstructions::ASR_reg: return "asr " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2);
	case     TInstructions::ADC: return "adc " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2);
	case     TInstructions::SBC: return "sbc " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2);
	case     TInstructions::ROR: return "ror " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2);
	case     TInstructions::TST: return "tst " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2);
	case     TInstructions::NEG: return "neg " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2);
	case TInstructions::CMP_reg: return "cmp " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2);
	case     TInstructions::CMN: return "cmn " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2);
	case     TInstructions::ORR: return "orr " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2);
	case     TInstructions::MUL: return "mul " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2);
	case     TInstructions::BIC: return "bic " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2);
	case     TInstructions::MVN: return "mvn " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2);	

	case TInstructions::ADD_imm_pc: return "add " + Disassemble_Reg(ir.operand2) + ", PC, #" + std::to_string(ir.operand1 * 4);
	case TInstructions::ADD_imm_sp: return "add " + Disassemble_Reg(ir.operand2) + ", SP, #" + std::to_string(ir.operand1 * 4);

	case TInstructions::ADD_inc_sp: return "add SP, #" + std::to_string(ir.operand1 * 4);
	case TInstructions::SUB_dec_sp: return "sub SP, #" + std::to_string(ir.operand1 * 4);
	
	case TInstructions::ADD_hig_reg: return "add " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2);
	case TInstructions::CMP_hig_reg: return "cmp " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2);
	case TInstructions::MOV_hig_reg: return "mov " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2);
	}

	throw std::string("Could not disassemble Thumb instruction");
}

std::string Disassembler::Disassemble_Branch_Signed_Offset(s32 offset) {
	std::string sign = offset >= 0 ? "+" : "-";
	return sign + "#" + std::to_string(abs(offset));
}
