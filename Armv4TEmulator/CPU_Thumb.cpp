#include "CPU.h"
#include "Utils.h"
#include "Decoder/Decoder.h"

void CPU::Execute(IR_Thumb& ir) {
	switch (ir.type) {
	case TInstructionType::Data_Processing_1: throw std::string("Unimplemented opcode"); break;
	case TInstructionType::Branch: throw std::string("Unimplemented opcode"); break;
	case TInstructionType::Multiply: throw std::string("Unimplemented opcode"); break;
	case TInstructionType::Status_Regsiter_Access: throw std::string("Unimplemented opcode"); break;
	case TInstructionType::Load_Store: throw std::string("Unimplemented opcode"); break;
	case TInstructionType::Load_Store_Multiple: throw std::string("Unimplemented opcode"); break;
	case TInstructionType::Exception_Generating: throw std::string("Unimplemented opcode"); break;
	}
}


void CPU::Data_Processing(IR_Thumb& ir) {

}