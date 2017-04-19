#include <iostream>
#include "../Common.h"
#include "../Decoder/Decoder.h"
#include "../Disassembler/Disassembler.h"
#include "../Utils.h"

#include "../catch/catch.hpp"

TEST_CASE("WIP", "[Disassembler]") {
	IR_ARM ir;
	Decoder::Decode(ir, 0xE2853004);
	std::cout << Disassmbler::Disassemble(ir);
}