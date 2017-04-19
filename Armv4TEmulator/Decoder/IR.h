#pragma once

#include "../Common.h"

//see "4.2 ARM instructions and architecture versions" page 213
enum class Instructions {
	ADC,
	ADD,
	AND,
	B,
	BIC,
	BL,
	BLX_1,
	BLX_2,
	BX,
	CDP,
	CMN,
	CMP,
	EOR,
	LDC,
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
	Instructions instr;
	Conditions cond;
	u32 operand1;
	u32 operand2;
	u32 operand3;
	Shifter_op shifter_operand;
};
