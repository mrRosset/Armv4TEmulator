#include "Disassembler.h"


void Disassembler::Disassemble(IR_ARM & ir, u32 instr)
{
	ir.cond = static_cast<Conditions>((instr >> 28) & 0b1111);

}
