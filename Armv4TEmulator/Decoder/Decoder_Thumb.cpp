#include <string>
#include "Decoder.h"
#include "../Utils.h"

void Decoder::Decode(IR_Thumb& ir, u16 instr){
	//TODO: find a better way to detect if there is a condition field.
	ir.cond = Conditions::AL;

	if (((instr >> 13) & 0b111) == 0b000) {
		if (((instr >> 11) & 0b11) == 0b11) {
			Decode_Add_Sub_reg_imm(ir, instr);
			return;
		}
		else {
			Decode_Shift_Imm(ir, instr);
		}
	}
	
	switch ((instr >> 13) & 0b111) {
		case 0b111: Decode_Unconditionnal_Branch(ir, instr);  return;
	}
	
	if (((instr >> 8) & 0b11111111) == 0b01000111) {
		Decode_Branch_With_Exchange(ir, instr);
		return;
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
			ir.instr = TInstructions::B_cond;
			ir.cond = static_cast<Conditions>(bits11_8);
			ir.operand1 = instr & 0xFF; // signed immed
		}
		return;
	}

}

void Decoder::Decode_Unconditionnal_Branch(IR_Thumb& ir, u16 instr) {
	ir.type = InstructionType::Branch;
	unsigned H = (instr >> 11) & 0b11;

	switch (H) {
	case 0b00: ir.instr = TInstructions::B_imm; break;
	case 0b10: ir.instr = TInstructions::BL_high; break;
	case 0b01: ir.instr = TInstructions::BLX_imm; break;
	case 0b11: ir.instr = TInstructions::BL; break;
	}

	ir.operand1 = instr & 0x7FF; //immed
	ir.operand2 = H;
}

void Decoder::Decode_Branch_With_Exchange(IR_Thumb& ir, u16 instr) {
	ir.type = InstructionType::Branch;

	switch (getBit(instr, 7)) {
	case 0: ir.instr = TInstructions::BX; break;
	case 1: ir.instr = TInstructions::BLX_reg; break;
	}

	ir.operand1 = (instr >> 3) & 0xF; //H2|Rm
}

void Decoder::Decode_Add_Sub_reg_imm(IR_Thumb& ir, u16 instr) {
	ir.type = InstructionType::Data_Processing;
	
	switch ((instr >> 9) & 0b11) {
	case 0b00: ir.instr = TInstructions::ADD_reg; break;
	case 0b01: ir.instr = TInstructions::SUB_reg; break;
	case 0b10: ir.instr = TInstructions::ADD_imm; break;
	case 0b11: ir.instr = TInstructions::SUB_imm; break;
	}

	ir.operand1 = instr & 0b111; //Rd
	ir.operand2 = (instr >> 3) & 0b111; //Rn
	ir.operand3 = (instr >> 6) & 0b111; //Rm or immed
}

void Decoder::Decode_Shift_Imm(IR_Thumb& ir, u16 instr) {
	ir.type = InstructionType::Data_Processing;

	switch ((instr >> 11) & 0b11) {
	case 0b00: ir.instr = TInstructions::LSL_imm; break;
	case 0b01: ir.instr = TInstructions::LSR_imm; break;
	case 0b10: ir.instr = TInstructions::ASR_imm; break;
	}

	ir.operand1 = instr & 0b111; //Rd
	ir.operand2 = (instr >> 3) & 0b111; //Rm
	ir.operand3 = (instr >> 6) & 0b11111; //shift imm
}