#include <iostream>
#include "../Common.h"
#include "../Decoder/Decoder.h"
#include "../Disassembler/Disassembler.h"
#include "../Utils.h"

#include "../catch/catch.hpp"


std::string Disassemble(u16 instr) {
	IR_Thumb ir;
	Decoder::Decode(ir, instr);
	return Disassembler::Disassemble(ir);
}

