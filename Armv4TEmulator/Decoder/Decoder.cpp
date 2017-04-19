#include <string>
#include "Decoder.h"
#include "../Utils.h"


void Decoder::Decode(IR_ARM & ir, u32 instr) {
	ir.cond = static_cast<Conditions>((instr >> 28) & 0b1111);
	
	unsigned bits28_25 = (instr >> 25) & 0b111;
	switch (bits28_25) {
	case 0b000:  // Data processing + Miscellaneous
	{
		unsigned bit4 = (instr >> 4) & 0b1;
		unsigned bit7 = (instr >> 7) & 0b1;
		unsigned bit20 = (instr >> 20) & 0b1;
		unsigned bit24_23 = (instr >> 23) & 0b11;

		if (bit24_23 == 0b10 && bit20 == 0) {
			unsigned bit27_4 = (instr >> 4) & 0xFFFFFF;
			if (bit27_4 == 0b000100101111111111110001) {
				Decode_Branch(ir, instr);
			}
			throw "3-3 not done";
		}
		else if (bit7 == 1 && bit4 == 1) {
			throw "3-2 not done";
		}
		else {
			Decode_Data_Processing(ir, instr);
		}
		break;
	}
	case 0b001:; // Data processing immediate + Move immediate to status register
	{
		unsigned bit21_20 = (instr >> 20) & 0b11;
		unsigned bit24_23 = (instr >> 23) & 0b11;
		if (bit24_23 == 0b10 && bit21_20 == 0b00) {
			throw "Undefined instruction are not emulated";
		}
		else if (bit24_23 == 0b10 && bit21_20 == 10) {
			throw "Move immediate to status register not done";
		}
		else {
			Decode_Data_Processing(ir, instr);
		}

		break;
	}
	case 0b010: // Load / store immediate offset
	case 0b011: // Load / store register offset
	case 0b100: // Load/store multiple
	case 0b101: Decode_Branch(ir, instr); break; // Branch and branch with link
	case 0b110: // Coprocessor load/store and double register transfers[<- does it exist without the DSP extension ?
	case 0b111: // Coprocessor + Software interrupts
	default: throw std::string("Unimplemented opcode");
	}

}


void Decoder::Decode_Data_Processing(IR_ARM & ir, u32 instr) {
	Decode_Shifter_operand(ir, instr);
	
	switch ((instr >> 21) & 0xF) {
	case 0b0000: ir.instr = Instructions::AND; break;
	case 0b0001: ir.instr = Instructions::EOR; break;
	case 0b0010: ir.instr = Instructions::SUB; break;
	case 0b0011: ir.instr = Instructions::RSB; break;
	case 0b0100: ir.instr = Instructions::ADD; break;
	case 0b0101: ir.instr = Instructions::ADC; break;
	case 0b0110: ir.instr = Instructions::SBC; break;
	case 0b0111: ir.instr = Instructions::RSC; break;
	case 0b1000: ir.instr = Instructions::TST; break;
	case 0b1001: ir.instr = Instructions::TEQ; break;
	case 0b1010: ir.instr = Instructions::CMP; break;
	case 0b1011: ir.instr = Instructions::CMN; break;
	case 0b1100: ir.instr = Instructions::ORR; break;
	case 0b1101: ir.instr = Instructions::MOV; break;
	case 0b1110: ir.instr = Instructions::BIC; break;
	case 0b1111: ir.instr = Instructions::MVN; break;
	}

	ir.operand1 = (instr >> 20) & 0b1;
	ir.operand2 = (instr >> 12) & 0xF;
	ir.operand3 = (instr >> 16) & 0xF;
}

void Decoder::Decode_Shifter_operand(IR_ARM& ir, u32 instr) {
	if (getBit(instr, 25)) {
		ir.shifter_operand = { Shifter_type::Immediate, (instr >> 8) & 0xF, instr & 0xFF };
		return;
	}

	unsigned shift_imm = (instr >> 7) & 0b11111;
	unsigned Rs = (instr >> 8) & 0xF;
	unsigned Rm = instr & 0xF;

	switch ((instr >> 4) & 0b111) {
	case 0b000: if(shift_imm == 0) ir.shifter_operand = { Shifter_type::Register, Rm};
				else ir.shifter_operand = { Shifter_type::LSL_imm, Rm, shift_imm };
				break;
	case 0b001: ir.shifter_operand = { Shifter_type::LSL_reg, Rm, Rs }; break;
	case 0b010: ir.shifter_operand = { Shifter_type::LSR_imm, Rm, shift_imm }; break;
	case 0b011: ir.shifter_operand = { Shifter_type::LSR_reg, Rm, Rs }; break;
	case 0b100: ir.shifter_operand = { Shifter_type::ASR_imm, Rm, shift_imm }; break;
	case 0b101: ir.shifter_operand = { Shifter_type::ASR_reg, Rm, Rs }; break;
	case 0b110: if(shift_imm == 0) ir.shifter_operand = { Shifter_type::RRX, Rm};
				else ir.shifter_operand = { Shifter_type::ROR_imm, Rm, shift_imm };
				break;
	case 0b111: ir.shifter_operand = { Shifter_type::ROR_reg, Rm, Rs }; break;
	}
}

void Decoder::Decode_Branch(IR_ARM& ir, u32 instr) {
	switch ((instr >> 24) & 0xF) {
	case 0b1010: ir.instr = Instructions::B; ir.operand1 = instr & 0xFFFFFF; break;
	case 0b1011: ir.instr = Instructions::BL; ir.operand1 = instr & 0xFFFFFF; break;
	case 0b0001: ir.instr = Instructions::BX; ir.operand1 = instr & 0xF; break;
	}
}