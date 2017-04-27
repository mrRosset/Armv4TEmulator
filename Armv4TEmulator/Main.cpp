#include <string>
#include <iostream>

#include "Common.h"
#include "Decoder\IR.h"
#include "Decoder\Decoder.h"
#include "Disassembler\Disassembler.h"

#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"


bool loadFile(std::string path, std::vector<u8>& data) {

	std::ifstream stream(path, std::ios::binary);
	if (!stream) {
		return false;
	}

	stream.seekg(0, std::ios::end);
	u64 length = stream.tellg();
	stream.seekg(0, std::ios::beg);

	data.resize(length);

	for (u64 i = 0; i < length; i++)
	{
		data[i] = stream.get();
	}

	stream.close();
	return true;
}

int main(int argc, char* argv[])
{
	if (argc > 1) {
		if (std::string(argv[1]) == "-t") {
			argv[1] = "";
			int result = Catch::Session().run(argc, argv);
			return (result < 0xff ? result : 0xff);
		}
		else {
			std::vector<u8> code;
			if (!loadFile(argv[1], code)) {
				return -1;
			}

			for (int i = 0; i < code.size() / 4; i+=4) {
				u32 instr = (code[i+3] << 24) | (code[i+2] << 16) | (code[i+1] << 8) | code[i];
				IR_ARM ir;
				Decoder::Decode(ir, instr);
				std::cout << Disassembler::Disassemble(ir) << "\r\n";
			}


		}
	}


	return 0;
}