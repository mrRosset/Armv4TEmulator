#pragma once
#include "..\Common.h"
#include "IR.h"

namespace Disassembler {
	void Disassemble(IR_ARM& ir, u32 isntr);
	void Disassemble_Data_Processing(IR_ARM& ir, u32 instr);
	void Disassemble_Shifter_operand(IR_ARM& ir, u32 instr);
}