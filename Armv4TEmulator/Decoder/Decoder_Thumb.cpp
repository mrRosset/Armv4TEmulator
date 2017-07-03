#include <string>
#include "Decoder.h"

void Decoder::Decode(IR_Thumb& ir, u16 instr){
	//TODO: find a better way to detect if there is a condition field.
	ir.cond = Conditions::AL;

	switch ((instr >> 13) & 0b111) {
	case 0b111: Decode_Branch(ir, instr);  return;
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
			ir.operand1 = instr & 0xFF; // signed immed
		}
		return;
	}

}

void Decoder::Decode_Branch(IR_Thumb& ir, u16 instr) {
	//TODO: 6.3.3
	ir.type = InstructionType::Branch;
	unsigned H = (instr >> 11) & 0b11;

	switch (H) {
	case 0b00: ir.instr = TInstructions::B; break;
	case 0b10: ir.instr = TInstructions::BL; break;
	case 0b01: ir.instr = TInstructions::BLX; break;
	case 0b11: ir.instr = TInstructions::BL; break;
	}

	ir.operand1 = instr & 0x7FF; //immed
	ir.operand2 = H;
}