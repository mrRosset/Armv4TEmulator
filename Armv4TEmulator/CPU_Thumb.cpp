#include "CPU.h"
#include "Utils.h"
#include "Decoder/Decoder.h"

void CPU::Execute(IR_Thumb& ir) {
	switch (ir.type) {
	case InstructionType::Data_Processing: throw std::string("Unimplemented opcode"); break;
	case InstructionType::Branch: throw std::string("Unimplemented opcode"); break;
	case InstructionType::Multiply: throw std::string("Unimplemented opcode"); break;
	case InstructionType::Status_Regsiter_Access: throw std::string("Unimplemented opcode"); break;
	case InstructionType::Load_Store: throw std::string("Unimplemented opcode"); break;
	case InstructionType::Load_Store_Multiple: throw std::string("Unimplemented opcode"); break;
	case InstructionType::Semaphore: throw std::string("Unimplemented opcode"); break;
	case InstructionType::Exception_Generating: throw std::string("Unimplemented opcode"); break;
	case InstructionType::Coprocessor: throw std::string("Unimplemented opcode"); break;
	case InstructionType::Extensions: throw std::string("Unimplemented opcode"); break;
	}
}
