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

TEST_CASE("Branch", "[Disassembler][Thumb]") {
	REQUIRE(Disassemble(0xDAAF) == "bge -#158");
	REQUIRE(Disassemble(0xD450) == "bmi +#164");

	REQUIRE(Disassemble(0xE2AA) == "b +#1368");
	REQUIRE(Disassemble(0xE010) == "b +#36");

	REQUIRE(Disassemble(0xFAAA) == "bl #1364");
	REQUIRE(Disassemble(0xF2AA) == "bl high +#2793476");

	REQUIRE(Disassemble(0x4770) == "bx lr");
	REQUIRE(Disassemble(0x4730) == "bx r6");
}