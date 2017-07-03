#include <string>
#include "Decoder.h"

void Decoder::Decode(IR_Thumb& ir, u16 instr){
	//TODO: find a better way to detect if there is a condition field.
	ir.cond = Conditions::AL;

	switch ((instr >> 13) & 0b111) {
	}
	
	switch ((instr >> 12) & 0b1111) {
	case 0b0101: return;
	case 0b0110:
	case 0b0111: return;
	case 0b1000: return;
	case 0b1001: return;
	case 0b1010: return;
	case 0b1011: return;
	case 0b1100: return;
	case 0b1101: 
		unsigned bits11_8 = (instr >> 8) & 0xF;
		switch (bits11_8) {
		case 0b1110: throw std::string("Undefined instruction");
		case 0b1111: break; //Software interrupts
		default:
			ir.type = InstructionType::Branch;
			ir.instr = TInstructions::B;
			ir.cond = static_cast<Conditions>(bits11_8);
			ir.operand1 = instr & 0xFF;
		}
		return;
	}

}
