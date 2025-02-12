#ifndef __OPERAND_H__
#define __OPERAND_H__

enum { OP_TYPE_REG, OP_TYPE_IMM, OP_TYPE_JUMP };

#define OP_STR_SIZE 40

typedef struct {
	uint32_t type;
	union {
		uint32_t reg;
		uint32_t imm;
		int32_t simm;
		int32_t instr_index;
	};
	uint32_t val;
} Operand;

typedef struct {
	uint32_t opcode;
	uint32_t func;
	uint32_t rt;
	uint32_t rs;
	Operand src1, src2, src3, dest;
} Operands;

#endif
