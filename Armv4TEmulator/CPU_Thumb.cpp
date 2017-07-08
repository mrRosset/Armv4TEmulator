#include "CPU.h"
#include "Utils.h"
#include "Decoder/Decoder.h"

void CPU::Execute(IR_Thumb& ir) {
	switch (ir.type) {
	case TInstructionType::Data_Processing_1: Data_Processing_1_2(ir); break;
	case TInstructionType::Data_Processing_2: Data_Processing_1_2(ir); break;
	case TInstructionType::Branch: throw std::string("Unimplemented opcode"); break;
	case TInstructionType::Multiply: throw std::string("Unimplemented opcode"); break;
	case TInstructionType::Status_Regsiter_Access: throw std::string("Unimplemented opcode"); break;
	case TInstructionType::Load_Store: throw std::string("Unimplemented opcode"); break;
	case TInstructionType::Load_Store_Multiple: throw std::string("Unimplemented opcode"); break;
	case TInstructionType::Exception_Generating: throw std::string("Unimplemented opcode"); break;
	}
}


void CPU::Data_Processing_1_2(IR_Thumb& ir) {
	u16& Rd = ir.operand1;
	u16& Rn = ir.operand2;
	u16& Rm = ir.operand3;
	u16& immed = ir.operand3;

	auto fun_Rd_31 = [&]()->bool {return !!getBit(gprs[Rd], 31); };
	auto fun_Rd_0 = [&]()->bool {return gprs[Rd] == 0; };

	switch (ir.instr) {
	case TInstructions::ADD_reg: DP_Instr1(Rd, gprs[Rn] + gprs[Rm], fun_Rd_31, fun_Rd_0, [&]()->bool {return CarryFrom(gprs[Rn], gprs[Rm]); }, [&]()->bool {return OverflowFromAdd(gprs[Rn], gprs[Rm]); }); break;
	case TInstructions::SUB_reg: DP_Instr1(Rd, gprs[Rn] - gprs[Rm], fun_Rd_31, fun_Rd_0, [&]()->bool {return !BorrowFromSub(gprs[Rn], gprs[Rm]); }, [&]()->bool {return OverflowFromSub(gprs[Rn], gprs[Rm]); }); break;
	case TInstructions::ADD_imm: DP_Instr1(Rd, gprs[Rn] + immed, fun_Rd_31, fun_Rd_0, [&]()->bool {return CarryFrom(gprs[Rn], immed); }, [&]()->bool {return OverflowFromAdd(gprs[Rn], immed); }); break;
	case TInstructions::SUB_imm: DP_Instr1(Rd, gprs[Rn] - immed, fun_Rd_31, fun_Rd_0, [&]()->bool {return !BorrowFromSub(gprs[Rn], immed); }, [&]()->bool {return OverflowFromSub(gprs[Rn], immed); }); break;
	}
}

void CPU::Data_Processing_3(IR_Thumb& ir) {
	u16& immed = ir.operand1;
	u16& Rd = ir.operand2;

	auto fun_Rd_31 = [&]()->bool {return !!getBit(gprs[Rd], 31); };
	auto fun_Rd_0 = [&]()->bool {return gprs[Rd] == 0; };

	switch (ir.instr) {
	case TInstructions::ADD_lar_imm: DP_Instr1(Rd, immed + gprs[Rd], fun_Rd_31, fun_Rd_0, [&]()->bool {return CarryFrom(immed, gprs[Rd]); }, [&]()->bool {return OverflowFromAdd(immed, gprs[Rd]); }); break;
	case TInstructions::SUB_lar_imm: DP_Instr1(Rd, gprs[Rd] - immed, fun_Rd_31, fun_Rd_0, [&]()->bool {return !BorrowFromSub(gprs[Rd], immed); }, [&]()->bool {return OverflowFromSub(gprs[Rd], immed); }); break;
	case TInstructions::MOV_imm: DP_Instr1(gprs[Rd], immed, fun_Rd_31, fun_Rd_0, fun_C, fun_V); break;
	case TInstructions::CMP_imm: DP_Instr2(gprs[Rd] - immed, fun_r_31, fun_r_0, [&](u32 r)->bool {return !BorrowFromSub(gprs[Rd], immed); }, [&](u32 r)->bool {return OverflowFromSub(gprs[Rd], immed); }); break;
	}
}

void CPU::Data_Processing_4(IR_Thumb& ir) {
	u16& Rd = ir.operand1;
	u16& Rm = ir.operand2;
	u16& immed = ir.operand3;

	bool flag_C = cpsr.flag_C;
	u32 vRm = gprs[Rm];

	auto fun_Rd_31 = [&]()->bool {return !!getBit(gprs[Rd], 31); };
	auto fun_Rd_0 = [&]()->bool {return gprs[Rd] == 0; };

	//TODO: Is C flag using new values or old ? seems like it should be the old
	switch (ir.instr) {
	case TInstructions::LSL_imm: DP_Instr1(Rd, gprs[Rm] << immed, fun_Rd_31, fun_Rd_0, [&]()->bool {return immed == 0 ? flag_C : !!getBit(vRm, 32 - immed); }, fun_V); break;
	case TInstructions::LSR_imm: DP_Instr1(Rd, immed == 0 ? 0 : gprs[Rm] >> immed, fun_Rd_31, fun_Rd_0, [&]()->bool {return immed == 0 ? !!getBit(vRm, 31) : !!getBit(vRm, immed - 1); }, fun_V); break; break;
	case TInstructions::ASR_imm: DP_Instr1(Rd, immed == 0 ? (getBit(gprs[Rm], 31) == 0 ? 0 : 0xFFFFFFFF): s32(gprs[Rm]) >> immed, fun_Rd_31, fun_Rd_0, [&]()->bool {return immed == 0 ? !!getBit(vRm, 31) : !!getBit(vRm, immed - 1); }, fun_V); break; break;
	}
}

inline void CPU::DP_Instr1(unsigned Rd, u32 result, std::function<bool()> N, std::function<bool()> Z, std::function<bool()> C, std::function<bool()> V) {
	gprs[Rd] = result;
	cpsr.flag_N = N();
	cpsr.flag_Z = Z();
	cpsr.flag_C = C();
	cpsr.flag_V = V();
}