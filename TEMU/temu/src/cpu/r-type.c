#include "helper.h"
#include "monitor.h"
#include "reg.h"

extern uint32_t instr;
extern char assembly[80];

/* decode R-type instrucion */
static void decode_r_type(uint32_t instr) {
	//rs
	op_src1->type = OP_TYPE_REG;
	op_src1->reg = (instr & RS_MASK) >> (RT_SIZE + IMM_SIZE);
	op_src1->val = reg_w(op_src1->reg);
	//rt	
	op_src2->type = OP_TYPE_REG;
	op_src2->imm = (instr & RT_MASK) >> (RD_SIZE + SHAMT_SIZE + FUNC_SIZE);
	op_src2->val = reg_w(op_src2->reg);
	//rd
	op_dest->type = OP_TYPE_REG;
	op_dest->reg = (instr & RD_MASK) >> (SHAMT_SIZE + FUNC_SIZE);
	//sa
	op_src3->type = OP_TYPE_REG;
	op_src3->imm = (instr & SHAMT_MASK) >> FUNC_SIZE;
	op_src3->val = op_src3->imm;
}


uint32_t sign_extend32(uint32_t imm);

uint32_t sign_extend32_8(uint32_t imm);


make_helper(add) {//Y

	decode_r_type(instr);
	uint64_t tmp = ((op_src1->val << 1) & 0x0000000100000000) + op_src1->val + ((op_src2->val << 1) & 0x0000000100000000) + op_src2->val;
	if(tmp !=(tmp & 0xFFFFFFFEFFFFFFFF)){
		sprintf(assembly,"Exception:Integer Overflow!");
		SignalException(Ov);
	}
	reg_w(op_dest->reg) = tmp & 0x00000000FFFFFFFF;
	sprintf(assembly, "add   %s,   %s,   %s", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), REG_NAME(op_src2->reg));
}

make_helper(addu) {
	decode_r_type(instr);
	reg_w(op_dest->reg) = (op_src1->val + op_src2->val);
	sprintf(assembly, "addu   %s,   %s,   %s", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), REG_NAME(op_src2->reg));
}

make_helper(sub) {//Y

	decode_r_type(instr);
	uint64_t tmp = ((op_src1->val << 1) & 0x0000000100000000) + op_src1->val - ((op_src2->val << 1) & 0x0000000100000000) - op_src2->val;
	if(tmp !=(tmp & 0xFFFFFFFEFFFFFFFF)){
		sprintf(assembly,"Exception:Integer Overflow!");
		SignalException(Ov);
	}
	reg_w(op_dest->reg) = tmp & 0x00000000FFFFFFFF;
	sprintf(assembly, "sub   %s,   %s,   %s", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), REG_NAME(op_src2->reg));
}

make_helper(subu) {

	decode_r_type(instr);
	reg_w(op_dest->reg) = (op_src1->val - op_src2->val);
	sprintf(assembly, "subu   %s,   %s,   %s", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), REG_NAME(op_src2->reg));
}

make_helper(slt) {

	decode_r_type(instr);
	if(op_src1->val>>31==1 && op_src2->val>>31==0 ){
		reg_w(op_dest->reg) = 1;
	}
	else if(op_src1->val>>31==0 && op_src2->val>>31==1 ){
		reg_w(op_dest->reg) = 0;
	}
	else if(op_src1->val>>31==0 && op_src2->val>>31==0 ){
		if(op_src1->val < op_src2->val)
			reg_w(op_dest->reg) = 1;
		else
			reg_w(op_dest->reg) = 0;
	}
	else{//(op_src1->val>>31==1 && sign_extend32(op_src2->val)>>31==1 )
		if(op_src1->val < op_src2->val)
			reg_w(op_dest->reg) = 0;
		else
			reg_w(op_dest->reg) = 1;
	}
	sprintf(assembly, "slt   %s,   %s,   %s", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), REG_NAME(op_src2->reg));
}

make_helper(sltu) {

	decode_r_type(instr);
	if(op_src1->val < op_src2->val)
		reg_w(op_dest->reg) = 1;
	else
		reg_w(op_dest->reg) = 0;
	sprintf(assembly, "sltu   %s,   %s,   %s", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), REG_NAME(op_src2->reg));
}

make_helper(and) {

	decode_r_type(instr);
	reg_w(op_dest->reg) = (op_src1->val & op_src2->val);
	sprintf(assembly, "and   %s,   %s,   %s", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), REG_NAME(op_src2->reg));
}

make_helper(or) {

	decode_r_type(instr);
	reg_w(op_dest->reg) = (op_src1->val | op_src2->val);
	sprintf(assembly, "or   %s,   %s,   %s", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), REG_NAME(op_src2->reg));
}

make_helper(nor) {

	decode_r_type(instr);
	reg_w(op_dest->reg) = (op_src1->val | op_src2->val) ^ 0xFFFFFFFF;
	sprintf(assembly, "nor   %s,   %s,   %s", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), REG_NAME(op_src2->reg));
}

make_helper(xor) {

	decode_r_type(instr);
	reg_w(op_dest->reg) = (op_src1->val ^ op_src2->val);
	sprintf(assembly, "xor   %s,   %s,   %s", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), REG_NAME(op_src2->reg));
}

make_helper(sll) {

	decode_r_type(instr);
	reg_w(op_dest->reg) = (op_src2->val << op_src3->val);
	sprintf(assembly, "sll   %s,   %s,   %01d", REG_NAME(op_dest->reg), REG_NAME(op_src2->reg), op_src3->imm);
}

make_helper(srl) {

	decode_r_type(instr);
	reg_w(op_dest->reg) = (op_src2->val >> op_src3->val);
	sprintf(assembly, "srl   %s,   %s,   %01d", REG_NAME(op_dest->reg), REG_NAME(op_src2->reg), op_src3->imm);
}

make_helper(sra) {

	decode_r_type(instr);
	reg_w(op_dest->reg) = (op_src2->val >> op_src3->val);
	if(op_src2->val >> 31 == 1){
		reg_w(op_dest->reg) = (0xFFFFFFFF << op_src3->val) | reg_w(op_dest->reg);
	}
	sprintf(assembly, "sra   %s,   %s,   %01d", REG_NAME(op_dest->reg), REG_NAME(op_src2->reg), op_src3->imm);
}

make_helper(sllv) {

	decode_r_type(instr);
	reg_w(op_dest->reg) = (op_src2->val << (op_src1->val & 0x0000001F));
	sprintf(assembly, "sllv   %s,   %s,   %s", REG_NAME(op_dest->reg), REG_NAME(op_src2->reg), REG_NAME(op_src1->reg));
}

make_helper(srlv) {

	decode_r_type(instr);
	reg_w(op_dest->reg) = (op_src2->val >> (op_src1->val & 0x0000001F));
	sprintf(assembly, "srlv   %s,   %s,   %s", REG_NAME(op_dest->reg), REG_NAME(op_src2->reg), REG_NAME(op_src1->reg));
}

make_helper(srav) {

	decode_r_type(instr);
	reg_w(op_dest->reg) = (op_src2->val >> (op_src1->val & 0x0000001F));
	if(op_src2->val >> 31 == 1){
		reg_w(op_dest->reg) = (0xFFFFFFFF << (op_src1->val & 0x0000001F)) | reg_w(op_dest->reg);
	}
	sprintf(assembly, "srav   %s,   %s,   %s", REG_NAME(op_dest->reg), REG_NAME(op_src2->reg), REG_NAME(op_src1->reg));
}

make_helper(mult) {
	decode_r_type(instr);
	uint64_t a=op_src1->val & 0x7FFFFFFF;
	uint64_t b=op_src2->val & 0x7FFFFFFF;
	uint64_t result32 = a * b;
	if((op_src1->val>>31)!=(op_src2->val>>31)){
		result32 = result32 | 0x8000000000000000;
	}
	cpu.hi = result32 >> 32;
	cpu.lo = result32 & 0x00000000FFFFFFFF;
	sprintf(assembly, "mult   %s,   %s", REG_NAME(op_src1->reg), REG_NAME(op_src2->reg));
}

make_helper(multu) {

	decode_r_type(instr);
	uint64_t a=op_src1->val;
	uint64_t b=op_src2->val;
	uint64_t result32 = a * b;
	cpu.hi = result32 >> 32;
	cpu.lo = result32 & 0x00000000FFFFFFFF;
	sprintf(assembly, "multu   %s,   %s", REG_NAME(op_src1->reg), REG_NAME(op_src2->reg));
}

make_helper(div) {

	decode_r_type(instr);
	if(op_src1->val>>31 == op_src2->val>>31){
		cpu.lo = (op_src1->val<<1) / (op_src2->val<<1);
	}
	else{//((op_src1->val>>31==1 && sign_extend32(op_src2->val)>>31==0 )||(op_src1->val>>31==0 && sign_extend32(op_src2->val)>>31==1 ))
		cpu.lo = 0x80000000 | (op_src1->val<<1) / (op_src2->val<<1);
	}
	cpu.hi = (op_src1->val<<1) % (op_src2->val<<1);
	sprintf(assembly, "div   %s,   %s", REG_NAME(op_src1->reg), REG_NAME(op_src2->reg));
}

make_helper(divu) {

	decode_r_type(instr);
	cpu.lo = op_src1->val / op_src2->val;
	cpu.hi = op_src1->val % op_src2->val;
	sprintf(assembly, "divu   %s,   %s", REG_NAME(op_src1->reg), REG_NAME(op_src2->reg));
}

make_helper(mfhi) {

	decode_r_type(instr);
	reg_w(op_dest->reg) = cpu.hi;
	sprintf(assembly, "mfhi   %s", REG_NAME(op_dest->reg));
}

make_helper(mflo) {

	decode_r_type(instr);
	reg_w(op_dest->reg) = cpu.lo;
	sprintf(assembly, "mflo   %s", REG_NAME(op_dest->reg));
}

make_helper(mthi) {

	decode_r_type(instr);
	cpu.hi = op_src1->val;
	sprintf(assembly, "mthi   %s", REG_NAME(op_src1->reg));
}

make_helper(mtlo) {

	decode_r_type(instr);
	cpu.lo = op_src1->val;
	sprintf(assembly, "mthi   %s", REG_NAME(op_src1->reg));
}

make_helper(jr) {
	decode_r_type(instr);
	cpu.pc = op_src1->val;
	sprintf(assembly, "jr   %s", REG_NAME(op_src1->reg));
}

make_helper(jalr) {
	decode_r_type(instr);
	reg_w(op_dest->reg) = cpu.pc + 8;
	cpu.pc = op_src1->val;
	sprintf(assembly, "jalr   %s,   %s", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg));
}

make_helper(syscall) {
	decode_r_type(instr);
	sprintf(assembly,"Exception:System Call!");
	SignalException(Sys);
}

make_helper(_break) {
	decode_r_type(instr);
	sprintf(assembly,"Exception:Break!");
	SignalException(Bp);
}