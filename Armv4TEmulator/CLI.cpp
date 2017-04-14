#include <iostream>
#include "Common.h"
#include "CPU.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch\catch.hpp"

TEST_CASE("Step without crashing", "[Global]") {
	CPU cpu;
	cpu.mem.write32(0, 0b11100000000000000000000000000000);
	cpu.Step();
}


TEST_CASE("Check Condition works correctly", "[ARM]") {
	//Rely on AND instruction working
	CPU cpu;
	//and r0, r1, r0
	cpu.mem.write32(0, 0b11100000000000010000000000000000);
	cpu.gprs[0] = 0b1100;
	cpu.gprs[1] = 0b1010;
	cpu.Step();
	REQUIRE(cpu.gprs[0] == 0b1000);
}

