#include <string>
#include <iostream>

#include "Common.h"
#include "CPU.h"
#include "Decoder/IR.h"
#include "Decoder/Decoder.h"
#include "Disassembler/Disassembler.h"

#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"


void loadFile(std::string path, std::vector<u8>& data) {

	#if defined(_WIN32)
	path = "\\\\?\\" + path;
	#endif

	std::ifstream stream(path, std::ios::binary | std::ios::ate);
	if (!stream) {
		throw std::string("Impossible to open file");
	}

	u64 length = stream.tellg();
	stream.seekg(0, std::ios::beg);

	if (data.size() < length) {
		data.resize(length);
	}
	if (!stream.read((char*)data.data(), length))
	{
		throw std::string("Error reading bytes from file");
	}

	stream.close();
}

void emulate(std::string path) {
	CPU cpu;
	cpu.mem.mem.resize(0x2FFF'FFFF);
	loadFile(path, cpu.mem.mem);

	//test
	cpu.gprs[Regs::PC] = 0x64;
	cpu.gprs[Regs::SP] = 0x3F'FFF;

	while (true) {
		for (int i = 0; i < 5; i++) {
			u16 instr = cpu.mem.read16(cpu.gprs[Regs::PC] + i*2);
			IR_Thumb ir;
			try {
				Decoder::Decode(ir, instr);
				std::string text = Disassembler::Disassemble(ir);
				std::cout << std::hex << cpu.gprs[Regs::PC] + i*2 << " - " << instr << std::dec << ": " << std::hex << Disassembler::Disassemble(ir) << std::endl;
			}
			catch (...) {
				std::cout << std::hex << cpu.gprs[Regs::PC] + i*2 << " - " << instr << std::dec << ": " << "Unkown instruction" << std::endl;
			}
		}

		std::cout << "\n\n";

		for (int i = 0; i < 15; i++) {
			std::cout << Disassembler::Disassemble_Reg(i) << ": " << std::hex << cpu.gprs[i] << std::dec << std::endl;
		}

		std::cout << "N:" << cpu.cpsr.flag_N << " Z:" <<cpu.cpsr.flag_Z << " C:" << cpu.cpsr.flag_C << " V:" << cpu.cpsr.flag_V << std::endl;

		std::cin.get();
		system("cls");
		//cpu.Step();
		cpu.gprs[Regs::PC] += 2;
	}

	for (int i = 0; i < cpu.mem.mem.size() / 4; i += 4) {
		u32 instr = cpu.mem.read32(i);
		IR_ARM ir;
		try {
			Decoder::Decode(ir, instr);
			std::cout << Disassembler::Disassemble(ir);
		}
		catch (...) {
			std::cout << "Unkown instruction";
		}
		std::cin.get();
	}
}

int main(int argc, char* argv[]) {
	if (argc > 1 && std::string(argv[1]) == "-p") {
		emulate(std::string(argv[2]));
	}
	else {
		//Start catch
		int result = Catch::Session().run(argc, argv);
		return (result < 0xff ? result : 0xff);
	}

	return 0;
}