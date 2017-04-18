#pragma once
#include "..\Common.h"
#include "IR.h"

namespace Disassembler {
	void Disassemble(IR_ARM& ir, u32 isntr);
}