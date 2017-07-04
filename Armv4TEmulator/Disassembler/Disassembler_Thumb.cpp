#include "Disassembler.h"
#include "../Utils.h"


std::string Disassembler::Disassemble(IR_Thumb& ir) {
	
	switch (ir.instr) {

	//Branch Instructions
	case TInstructions::B_cond: return "b" + Disassemble_Cond(ir.cond) + " " + Disassemble_Signed_Offset(SignExtend<s32>(ir.operand1 << 1, 9) + 4);
	case TInstructions::B_imm: return "b " + Disassemble_Signed_Offset(SignExtend<s32>(ir.operand1 << 1, 12) + 4);
	}
	
	return std::string();
}

std::string Disassembler::Disassemble_Signed_Offset(s16 offset) {
	std::string sign = offset >= 0 ? "+" : "-";
	return sign + "#" + std::to_string(abs(offset));
}
