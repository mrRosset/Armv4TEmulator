#pragma once
#include "..\Common.h"
#include "IR.h"

namespace Decoder {
	void Decode(IR_ARM& ir, u32 instr);
	void Decode_Data_Processing(IR_ARM& ir, u32 instr);
	void Decode_Shifter_operand(IR_ARM& ir, u32 instr);
	void Decode_Branch(IR_ARM& ir, u32 instr);
}