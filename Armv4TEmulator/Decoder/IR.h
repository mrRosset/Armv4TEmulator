#pragma once

#include "../Common.h"


namespace Regs {
	enum {
		/*
		Register R13 is normally used as a stack pointer and is also known as the SP. In the ARM instruction set,
		this is by convention only, as there are no defined instructions or other functionality which use R13 in a
		special-case manner. However, there are such instructions in the Thumb instruction set,
		*/
		SP = 13,
		LR = 14,
		/*
		Beware of all the special cases regarding R15. Also reading is a special thing. see page 38/39
		! see what implementation it is for STR and STM with R15 -> It's 12
		*/
		PC = 15
	};
}

enum class InstructionType {
	Data_Processing,
	Branch,
	Multiply,
	Status_Regsiter_Access,
	Load_Store,
	Load_Store_Multiple,
	Semaphore, // to check
	Exception_Generating, //to check
	Coprocessor, //to check
	Extensions //Is there only
};

//see "4.2 ARM instructions and architecture versions" page 213
enum class Instructions {
	ADC,
	ADD,
	AND,
	B,
	BIC,
	BL,
	BX,
	CDP,
	CMN,
	CMP,
	EOR,
	LDC,
	LDMIA,
	LDMIB,
	LDMDA,
	LDMDB,
	LDMFD,
	LDMFA,
	LDMED,
	LDMEA,
	LDM_1,
	LDM_2,
	LDM_3,
	LDR,
	LDRB,
	LDRBT,
	LDRH,
	LDRSB,
	LDRSH,
	LDRT,
	MCR,
	MLA,
	MOV,
	MRC,
	MRS,
	MSR,
	MUL,
	MVN,
	ORR,
	RSB,
	RSC,
	SBC,
	SMLAL,
	SMULL,
	STC,
	STMIA,
	STMIB,
	STMDA,
	STMDB,
	STMFD,
	STMFA,
	STMED,
	STMEA,
	STM_1,
	STM_2,
	STR,
	STRB,
	STRBT,
	STRH,
	STRT,
	SUB,
	SWI,
	SWP,
	SWPB,
	TEQ,
	TST,
	UMLAL,
	UMULL
};

enum class Conditions: u8{
	EQ    = 0b0000,
	NE    = 0b0001,
	CS_HS = 0b0010,
	CC_LO = 0b0011,
	MI    = 0b0100,
	PL    = 0b0101,
	VS    = 0b0110,
	VC    = 0b0111,
	HI    = 0b1000,
	LS    = 0b1001,
	GE    = 0b1010,
	LT    = 0b1011,
	GT    = 0b1100,
	LE    = 0b1101,
	AL    = 0b1110,
	NV    = 0b1111
};

enum Shifter_type {
	Immediate,
	Register,
	LSL_imm,
	LSL_reg,
	LSR_imm,
	LSR_reg,
	ASR_imm,
	ASR_reg,
	ROR_imm,
	ROR_reg,
	RRX
};

struct Shifter_op {
	Shifter_type type;
	u32 operand1;
	u32 operand2;
};

struct IR_ARM {
	InstructionType type;
	Instructions instr;
	Conditions cond;
	bool s;
	u32 operand1;
	u32 operand2;
	u32 operand3;
	u32 operand4;
	Shifter_op shifter_operand;
};