#pragma once

#include <string>
#include "../Common.h"
#include "../Decoder/IR.h"


namespace Disassembler {
	std::string Disassemble(IR_ARM& ir);
	std::string Disassemble_register(u32 reg);
	std::string Disassemble_Condition(IR_ARM& ir);
	std::string Disassemble_Shifter_Operand(Shifter_op& so);
	std::string Disassemble_Branch_Offset(u32 operand);
}