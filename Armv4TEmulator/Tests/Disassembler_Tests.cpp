#include <iostream>
#include "../Common.h"
#include "../Decoder/Decoder.h"
#include "../Disassembler/Disassembler.h"
#include "../Utils.h"

#include "../catch/catch.hpp"


std::string Disassemble(u32 instr) {
	IR_ARM ir;
	Decoder::Decode(ir, instr);
	return Disassembler::Disassemble(ir);
}

TEST_CASE("Disassemble Data Processing", "[Disassembler]") {
	REQUIRE(Disassemble(0x2201230F) == "andcs r2, r1, #1006632960");
}