#pragma once
#include "../Common.h"
#include "IR.h"

namespace Decoder {
	void Decode(IR_ARM& ir, u32 instr);
	void Decode_Data_Processing(IR_ARM& ir, u32 instr);
	void Decode_Shifter_operand(IR_ARM& ir, u32 instr);
	void Decode_Branch(IR_ARM& ir, u32 instr);
	void Decode_Multiply(IR_ARM& ir, u32 instr);
	void Decode_Status_Register(IR_ARM& ir, u32 instr);
	void Decode_Load_Store_W_UB(IR_ARM& ir, u32 instr);
	void Decode_Load_Store_H_SB(IR_ARM& ir, u32 instr);
}