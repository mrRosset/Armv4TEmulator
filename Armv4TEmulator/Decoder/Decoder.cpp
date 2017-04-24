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
		unsigned bit22 = (instr >> 22) & 0b1;
		unsigned bit24_23 = (instr >> 23) & 0b11;

		if (bit24_23 == 0b10 && bit20 == 0) {
			unsigned bit7_4 = (instr >> 4) & 0xF;
			switch (bit7_4) {
			case 0b0000: Decode_Status_Register(ir, instr); break;
			case 0b0001: if(bit22 == 0) Decode_Branch(ir, instr);
						 else throw "3-3 not done"; break;
			case 0b0011:
			case 0b0101:
			case 0b0111:
			//case 0b1xy0:
			default:
				throw "3-3 not done";
			}
		}
		else if (bit7 == 1 && bit4 == 1) {
			unsigned bit7_4 = (instr >> 4) & 0xF;
			if (bit7_4 == 0b1001) {
				Decode_Multiply(ir, instr);
			}
			else {
				throw "3-2 not done";
			}
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
		else if (bit24_23 == 0b10 && bit21_20 == 0b10) {
			Decode_Status_Register(ir, instr);
		}
		else {
			Decode_Data_Processing(ir, instr);
		}

		break;
	}
	case 0b010: Decode_Load_Store_W_UB(ir, instr); // Load / store immediate offset
	case 0b011: Decode_Load_Store_W_UB(ir, instr); // Load / store register offset
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

	ir.s = ((instr >> 20) & 0b1) == 1; //S
	ir.operand1 = (instr >> 12) & 0xF; //Rd
	ir.operand2 = (instr >> 16) & 0xF; //Rn
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

void Decoder::Decode_Multiply(IR_ARM& ir, u32 instr) {

	switch ((instr >> 21) & 0xF) {
	case 0b0000: ir.instr = Instructions::MUL; break;
	case 0b0001: ir.instr = Instructions::MLA; break;
	case 0b0100: ir.instr = Instructions::UMULL; break;
	case 0b0101: ir.instr = Instructions::UMLAL; break;
	case 0b0110: ir.instr = Instructions::SMULL; break;
	case 0b0111: ir.instr = Instructions::SMLAL; break;
	}

	ir.s = ((instr >> 20) & 0b1) == 1; //S
	ir.operand1 = instr & 0xF; //Rm
	ir.operand2 = (instr >> 8) & 0xF; //Rs
	ir.operand3 = (instr >> 12) & 0xF; //Rn or RdLo
	ir.operand4 = (instr >> 16) & 0xF; //Rd or RdHi
}

void Decoder::Decode_Status_Register(IR_ARM& ir, u32 instr) {
	switch ((instr >> 20) & 0b11111011) {
	case 0b00010000: ir.instr = Instructions::MRS; ir.operand2 = (instr >> 12) & 0xF; break;
	case 0b00110010: ir.instr = Instructions::MSR; ir.operand2 = (instr >> 16) & 0xF; ir.shifter_operand = { Shifter_type::Immediate, (instr >> 8) & 0xF, instr & 0xFF }; break;
	case 0b00010010: ir.instr = Instructions::MSR; ir.operand2 = (instr >> 16) & 0xF; ir.shifter_operand = { Shifter_type::Register,  instr & 0xFF }; break;
	}

	ir.operand1 = (instr >> 22) & 0b1; //R
}

void Decoder::Decode_Load_Store_W_UB(IR_ARM& ir, u32 instr) {

	switch ((instr >> 20) & 0b10111) {
	case 0b00000: ir.instr = Instructions::STR; break;
	case 0b00001: ir.instr = Instructions::LDR; break;
	case 0b00010: ir.instr = Instructions::STRT; break;
	case 0b00011: ir.instr = Instructions::LDRT; break;
	case 0b00100: ir.instr = Instructions::STRB; break;
	case 0b00101: ir.instr = Instructions::LDRB; break;
	case 0b00110: ir.instr = Instructions::STRBT; break;
	case 0b00111: ir.instr = Instructions::LDRBT; break;
	case 0b10001: ir.instr = Instructions::STR; break;
	case 0b10010: ir.instr = Instructions::STR; break;
	case 0b10011: ir.instr = Instructions::LDR; break;
	case 0b10100: ir.instr = Instructions::STRB; break;
	case 0b10101: ir.instr = Instructions::LDRB; break;
	case 0b10110: ir.instr = Instructions::STRB; break;
	case 0b10111: ir.instr = Instructions::LDRB; break;
	}

	ir.operand1 = (instr >> 12) & 0xF; //Rd
	ir.operand2 = (instr >> 16) & 0xF; //Rn
	ir.operand3 = (instr >> 21) & 0xF; //PUBW

	if (getBit(instr, 25) == 0) {
		ir.shifter_operand = { Shifter_type::Immediate, instr & 0xFFF };
	}
	else {
		u32 shift = (instr >> 5) & 0b11;
		u32 shift_imm = (instr >> 7) & 0x1F;

		if (shift == 0 && shift_imm == 0) {
			ir.shifter_operand = { Shifter_type::Register, instr & 0xF };
		}
		else {
			switch (shift) {
			case 0b00: ir.shifter_operand = { Shifter_type::LSL_imm, instr & 0xF,  shift_imm };
			case 0b01: ir.shifter_operand = { Shifter_type::LSR_imm, instr & 0xF,  shift_imm == 0 ? 32 : shift_imm };
			case 0b10: ir.shifter_operand = { Shifter_type::ASR_imm, instr & 0xF,  shift_imm == 0 ? 32 : shift_imm };
			case 0b11: ir.shifter_operand = { shift_imm == 0 ? Shifter_type::RRX : Shifter_type::ROR_imm, instr & 0xF,  shift_imm };
			}
		}
	}
	


}


/*For decoding co-processor:
TODO: Read and take care if necessary
	"The coprocessor double register transfer instructions are described in Chapter A10 Enhanced DSP Extension."
*/