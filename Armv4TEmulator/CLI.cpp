#include "Common.h"
#include "CPU.h"

int main()
{
	CPU cpu;
	cpu.mem.write32(0, 0b11100000000000000000000000000000);
	cpu.Step();
	return 0;
}