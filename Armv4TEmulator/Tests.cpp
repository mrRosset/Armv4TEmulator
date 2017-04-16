#include <iostream>
#include <bitset>
#include "Common.h"
#include "CPU.h"
#include "Utils.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch\catch.hpp"

/*
	Warning: All tests here were written by myself during developement of the
	emulator and were based on my understanding of the behavior on arm procesor.
	Those tests were not tried on any other emulator and any comprehension error
	of the reference manual will probably be present in those tests.
*/


TEST_CASE("Step without crashing", "[Global]") {
	CPU cpu;
	cpu.Step();
}

struct condition_test {
	u32 cond;
	bool flag_N, flag_Z, flag_C, flag_V;
	bool should_execute;
};

static const std::vector<condition_test> condition_tests = {
	{ 0b0000, false, true, false, false, true },
	{ 0b0000, false, false, false, false, false },
	{ 0b0001, false, false, false, false, true },
	{ 0b0001, false, true, false, false, false },
	{ 0b0010, false, false, true, false, true },
	{ 0b0010, false, false, false, false, false },
	{ 0b0011, false, false, false, false, true },
	{ 0b0011, false, false, true, false, false },
	{ 0b0100, true, false, false, false, true },
	{ 0b0100, false, false, false, false, false },
	{ 0b0101, false, false, false, false, true },
	{ 0b0101, true, false, false, false, false },
	{ 0b0110, false, false, false, true, true },
	{ 0b0110, false, false, false, false, false },
	{ 0b0111, false, false, false, false, true },
	{ 0b0111, false, false, false, true, false },
	{ 0b1000, false, false, true, false, true },
	{ 0b1000, false, true, true, false, false },
	{ 0b1000, false, false, false, false, false },
	{ 0b1000, false, true, false, true, false },
	{ 0b1001, false, true, false, false, true },
	{ 0b1001, false, true, true, false, true },
	{ 0b1001, false, false, false, false, true },
	{ 0b1001, false, false, true, true, false },
	{ 0b1010, false, false, false, false, true },
	{ 0b1010, true, false, false, true, true },
	{ 0b1010, false, false, false, true, false },
	{ 0b1010, true, false, false, false, false },
	{ 0b1011, false, false, false, true, true },
	{ 0b1011, true, false, false, false, true },
	{ 0b1011, true, false, false, true, false },
	{ 0b1011, false, false, false, false, false },
	{ 0b1100, false, false, false, false, true },
	{ 0b1100, true, false, false, true, true },
	{ 0b1100, false, false, false, true, false },
	{ 0b1100, true, false, false, false, false },
	{ 0b1100, false, true, false, false, false },
	{ 0b1100, true, true, false, true, false },
	{ 0b1100, false, true, false, true, false },
	{ 0b1100, true, true, false, false, false },
	{ 0b1101, false, true, false, true, true },
	{ 0b1101, true, true, false, false, true },
	{ 0b1101, true, true, false, true, false },
	{ 0b1101, false, true, false, false, false },
	{ 0b1101, false, false, false, true, false },
	{ 0b1101, true, false, false, false, false },
	{ 0b1101, true, false, false, true, false },
	{ 0b1101, false, false, false, false, false },
	{ 0b1110, false, false, false, false, true },
	{ 0b1110, true, true, true, true, true },
};

TEST_CASE("Condition fields works correctly", "[ARM]") {
	//Rely on AND instruction working
	for (const condition_test& tst: condition_tests) {
		CPU cpu;
		cpu.gprs[0] = 0b1100;
		cpu.gprs[1] = 0b1010;
		//and r0, r1, r0
		u32 op = 0b00000000000010000000000000000;

		cpu.cpsr.flag_N = tst.flag_N;
		cpu.cpsr.flag_Z = tst.flag_Z;
		cpu.cpsr.flag_C = tst.flag_C;
		cpu.cpsr.flag_V = tst.flag_V;
		//Unit
		REQUIRE(cpu.Check_Condition((tst.cond << 28) | op) == tst.should_execute);
		//Global
		cpu.mem.write32(0, (tst.cond << 28) | op);
		cpu.Step();
		if(tst.should_execute)
			REQUIRE(cpu.gprs[0] == 0b1000);
		else 
			REQUIRE(cpu.gprs[0] == 0b1100);
	}

}



TEST_CASE("Shifter Operand Immediate example from manual", "[ARM]") {
	CPU cpu;
	//S = 1
	u32 op = 0b00000010000100000001000000000000;
	u32 rotate_imm = 0xE;
	u32 immed_8 = 0x3F;
	u32 opcode = (rotate_imm << 8) | immed_8 | op;
	u32 shifter_op;
	bool shifter_carry;
	std::tie(shifter_op, shifter_carry) = cpu.shifter_operand(opcode, getBit(opcode, 25));

	REQUIRE(shifter_op == 0x3F0);
	REQUIRE(shifter_carry == false);

	u32 rotate_imm_2 = 0xF;
	u32 immed_8_2 = 0xFC;
	u32 opcode_2 = (rotate_imm_2 << 8) | immed_8_2 | op;
	u32 shifter_op_2;
	bool shifter_carry_2;
	std::tie(shifter_op_2, shifter_carry_2) = cpu.shifter_operand(opcode, getBit(opcode, 25));

	REQUIRE(shifter_carry == shifter_carry_2);
	REQUIRE(shifter_op == shifter_op_2);
}



