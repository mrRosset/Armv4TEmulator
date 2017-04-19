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

TEST_CASE("Disassemble Data Processing instr", "[Disassembler]") {
	REQUIRE(Disassemble(0x2201230F) == "andcs r2, r1, #1006632960");
}

TEST_CASE("Disassemble Branch instr", "[Disassembler]") {
	REQUIRE(Disassemble(0xEA000000) == "b +#8");
	REQUIRE(Disassemble(0xEAFFFFFE) == "b +#0");
	REQUIRE(Disassemble(0xEA000076) == "b +#480");
	REQUIRE(Disassemble(0xEB000000) == "bl +#8");
	REQUIRE(Disassemble(0xEBFFFFFE) == "bl +#0");
	REQUIRE(Disassemble(0xEB000076) == "bl +#480");
	REQUIRE(Disassemble(0x312FFF15) == "bxcc r5");
	REQUIRE(Disassemble(0x312FFF1E) == "bxcc lr");
}