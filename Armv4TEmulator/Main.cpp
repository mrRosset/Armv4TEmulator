#include <string>
#include <iostream>

#include "Common.h"
#include "Memory.h"
#include "Decoder\IR.h"
#include "Decoder\Decoder.h"
#include "Disassembler\Disassembler.h"

#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"


void loadFile(std::string path, std::vector<u8>& data) {

	std::ifstream stream(path, std::ios::binary | std::ios::ate);
	if (!stream) {
		throw std::string("Impossible to open file");
	}

	u64 length = stream.tellg();
	stream.seekg(0, std::ios::beg);

	data.resize(length);
	if (!stream.read((char*)data.data(), length))
	{
		throw std::string("Error reading bytes from file");
	}

	stream.close();
}

void emulate(std::string path) {
	Memory mem;
	loadFile(path, mem.mem);

	for (int i = 0; i < mem.mem.size() / 4; i += 4) {
		u32 instr = mem.read32(i);
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

int main(int argc, char* argv[]){
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