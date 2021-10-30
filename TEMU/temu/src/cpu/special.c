#include "helper.h"
#include "monitor.h"
#include "i-type.h"

extern char assembly[80];
extern uint32_t instr;


static void decode_imm_type(uint32_t instr) {
	//lable
	op_src1->type = OP_TYPE_REG;
	op_src1->imm = instr & INDEX_MASK;
	op_src1->val = op_src1->imm;
	//rt	
	op_src2->type = OP_TYPE_REG;
	op_src2->reg = (instr & RT_MASK) >> (RD_SIZE + SHAMT_SIZE + FUNC_SIZE);
	op_src2->val = reg_w(op_src2->reg);
	//rd	
	op_src3->type = OP_TYPE_REG;
	op_src3->reg = (instr & RS_MASK) >> (RT_SIZE + IMM_SIZE);
	op_src3->val = reg_0(op_src3->reg);
}

/* invalid opcode */
make_helper(inv) {

	uint32_t temp;
	temp = instr_fetch(pc, 4);
	
	uint8_t *p = (void *)&temp;
	printf("Exception: invalid opcode(pc = 0x%08x): %02x %02x %02x %02x ...\n\n", 
			pc, p[3], p[2], p[1], p[0]);

	printf("There are two cases which will trigger this unexpected exception:\n\
1. The instruction at pc = 0x%08x is not implemented.\n\
2. Something is implemented incorrectly.\n", pc);
	printf("Find this pc value(0x%08x) in the disassembling result to distinguish which case it is.\n\n", pc);
	SignalException(RI);
	assert(0);
}

/* stop temu */
make_helper(temu_trap) {

	printf("\33[1;31mtemu: HIT GOOD TRAP\33[0m at $pc = 0x%08x\n\n", cpu.pc);

	temu_state = END;

}

make_helper(j) {
	decode_imm_type(instr);
	cpu.pc = (op_src1->val << 2) + ((cpu.pc + 4) & 0xF0000000);
	sprintf(assembly, "j   ");
}

make_helper(jal) {
	decode_imm_type(instr);
	cpu.ra = cpu.pc + 8;
	cpu.pc = (op_src1->val << 2) + ((cpu.pc + 4) & 0xF0000000);
	sprintf(assembly, "jal   ");
}

make_helper(mfc0) {
	decode_imm_type(instr);
	reg_w(op_src2->reg) = op_src3->val;
	sprintf(assembly, "mfc0   %s,   %s", REG_NAME(op_src2->reg),  REG_NAME(op_src3->reg));
}

make_helper(mtc0) {
	decode_imm_type(instr);
	reg_0(op_src3->reg) = op_src2->val;
	sprintf(assembly, "mtc0   %s,   %s", REG_NAME(op_src3->reg),  REG_NAME(op_src2->reg));
}

make_helper(eret) {
	decode_imm_type(instr);
	cpu.pc = reg_0(13);
	status_exl_update(0);
	sprintf(assembly, "eret");
}