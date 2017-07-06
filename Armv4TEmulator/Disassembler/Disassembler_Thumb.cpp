#include "Disassembler.h"
#include "../Utils.h"


std::string Disassembler::Disassemble(IR_Thumb& ir) {
	
	switch (ir.instr) {

	//Branch Instructions
	case TInstructions::B_cond:  return "b" + Disassemble_Cond(ir.cond) + " " + Disassemble_Branch_Signed_Offset(SignExtend<s32>(ir.operand1 << 1, 9) + 4);
	case TInstructions::B_imm:   return "b " + Disassemble_Branch_Signed_Offset(SignExtend<s32>(ir.operand1 << 1, 12) + 4);
	case TInstructions::BL_high: return "bl(x) high " + Disassemble_Branch_Signed_Offset((SignExtend<s32>(ir.operand1, 11) << 12) + 4);
	case TInstructions::BL:      return "bl #" + std::to_string(ir.operand1 << 1);
	case TInstructions::BLX_imm: return "blx #" + std::to_string(ir.operand1 << 1);
	case TInstructions::BLX_reg: return "blx " + Disassemble_Reg(ir.operand1);
	case TInstructions::BX:      return "bx " + Disassemble_Reg(ir.operand1);
	
	//Data Processing Instructions
	case TInstructions::ADD_reg: return "add " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2) + ", " + Disassemble_Reg(ir.operand3);
	case TInstructions::SUB_reg: return "sub " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2) + ", " + Disassemble_Reg(ir.operand3);
	case TInstructions::ADD_imm: return "add " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2) + ", #" + std::to_string(ir.operand3);
	case TInstructions::SUB_imm: return "sub " + Disassemble_Reg(ir.operand1) + ", " + Disassemble_Reg(ir.operand2) + ", #" + std::to_string(ir.operand3);
	}

	return std::string();
}

std::string Disassembler::Disassemble_Branch_Signed_Offset(s32 offset) {
	std::string sign = offset >= 0 ? "+" : "-";
	return sign + "#" + std::to_string(abs(offset));
}
