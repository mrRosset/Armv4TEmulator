#include "Common.h"
#include "CPU.h"

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch\catch.hpp"

TEST_CASE("Step without crashing", "[Global]") {
	CPU cpu;
	cpu.mem.write32(0, 0b11100000000000000000000000000000);
	cpu.Step();
}
