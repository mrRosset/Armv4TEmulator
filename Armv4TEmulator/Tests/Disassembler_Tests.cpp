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
	
	REQUIRE(Disassemble(0xEB000344) == "bl +#3352");
	REQUIRE(Disassemble(0xEB000248) == "bl +#2344");
}

TEST_CASE("Disassemble Status Register access instr", "[Disassembler]") {
	REQUIRE(Disassemble(0xE14F3000) == "mrs r3, SPSR");
	REQUIRE(Disassemble(0xE10FD000) == "mrs sp, CPSR");

	REQUIRE(Disassemble(0xE16BF003) == "msr SPSR_fxc, r3");
	REQUIRE(Disassemble(0xE12BF005) == "msr CPSR_fxc, r5");
	REQUIRE(Disassemble(0xE365F603) == "msr SPSR_sc, #3145728");
	REQUIRE(Disassemble(0xE320FA23) == "msr CPSR_, #143360");
}

TEST_CASE("Disassemble Load/Store unsigned Byte/Word", "[Disassembler]") {
	REQUIRE(Disassemble(0xE5331048) == "ldr r1, [r3, #-72]!");
	REQUIRE(Disassemble(0xE5B31048) == "ldr r1, [r3, #72]!");
	REQUIRE(Disassemble(0xE4331048) == "ldrt r1, [r3], #-72");
	REQUIRE(Disassemble(0xE4B31048) == "ldrt r1, [r3], #72");
	REQUIRE(Disassemble(0xE4731048) == "ldrbt r1, [r3], #-72");
	REQUIRE(Disassemble(0xE4F31048) == "ldrbt r1, [r3], #72");
	REQUIRE(Disassemble(0xE4531048) == "ldrb r1, [r3], #-72");
	REQUIRE(Disassemble(0xE4D31048) == "ldrb r1, [r3], #72");
	REQUIRE(Disassemble(0xE5931048) == "ldr r1, [r3, #72]");
	REQUIRE(Disassemble(0xE5D31048) == "ldrb r1, [r3, #72]");
	REQUIRE(Disassemble(0xE4731000) == "ldrbt r1, [r3], #-0");

	REQUIRE(Disassemble(0xE5883000) == "str r3, [r8]");
}
