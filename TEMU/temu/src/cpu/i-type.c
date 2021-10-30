#include "helper.h"
#include "monitor.h"
#include "reg.h"

extern uint32_t instr;
extern char assembly[80];

/* decode I-type instrucion with unsigned immediate */
static void decode_imm_type(uint32_t instr) {
	//rs
	op_src1->type = OP_TYPE_REG;
	op_src1->reg = (instr & RS_MASK) >> (RT_SIZE + IMM_SIZE);
	op_src1->val = reg_w(op_src1->reg);
	//imm
	op_src2->type = OP_TYPE_IMM;
	op_src2->imm = instr & IMM_MASK;
	op_src2->val = op_src2->imm;
	//rt
	op_dest->type = OP_TYPE_REG;
	op_dest->reg = (instr & RT_MASK) >> (IMM_SIZE);
}

uint32_t sign_extend32(uint32_t imm){
    if(((imm&(0x1<<15))>>15)==1){
        return (0xFFFF0000 | imm);
    }
    else
        return imm;
}

uint32_t sign_extend32_8(uint32_t imm){
    if(((imm&(0x1<<7))>>7)==1){
        return (0xFFFFFF00 | imm);
    }
    else
        return imm;
}

void status_exl_update(int a){
	if(a == 1){
		cpu.cpu0[12] = cpu.cpu0[12] | 0x00000002;
	}
	if(a == 0){
		cpu.cpu0[12] = cpu.cpu0[12] & 0xFFFFFFFD;
	}
}

void SignalException(uint32_t x){
	//修改CP0cause寄存器的ExcCode字段
	status_exl_update(1);
	uint32_t a = x;
	cpu.cpu0[13] = cpu.cpu0[13] | (a << 2);
	cpu.cpu0[14] = cpu.pc + 4;
	cpu.pc = 0xbcf00380;
}

make_helper(addi) {//Y
	decode_imm_type(instr);
	uint64_t tmp = ((op_src1->val << 1) & 0x0000000100000000) + op_src1->val + sign_extend32(op_src2->val);
	if(tmp !=(tmp & 0xFFFFFFFEFFFFFFFF)){
		sprintf(assembly,"Exception:Integer Overflow!");
		SignalException(Ov);
	}
	reg_w(op_dest->reg) = tmp & 0x00000000FFFFFFFF;
	sprintf(assembly, "addi   %s,   %s,   0x%04x", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), op_src2->imm);
}

make_helper(addiu) {
	decode_imm_type(instr);
	reg_w(op_dest->reg) = op_src1->val + sign_extend32(op_src2->val);
	sprintf(assembly, "addiu   %s,   %s,   0x%04x", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), op_src2->imm);
}

make_helper(slti) {
	decode_imm_type(instr);
	if(op_src1->val>>31==1 && sign_extend32(op_src2->val)>>31==0 ){
		reg_w(op_dest->reg) = 1;
	}
	else if(op_src1->val>>31==0 && sign_extend32(op_src2->val)>>31==1 ){
		reg_w(op_dest->reg) = 0;
	}
	else if(op_src1->val>>31==0 && sign_extend32(op_src2->val)>>31==0 ){
		if(op_src1->val < sign_extend32(op_src2->val))
			reg_w(op_dest->reg) = 1;
		else
			reg_w(op_dest->reg) = 0;
	}
	else{//(op_src1->val>>31==1 && sign_extend32(op_src2->val)>>31==1 )
		if(op_src1->val < sign_extend32(op_src2->val))
			reg_w(op_dest->reg) = 0;
		else
			reg_w(op_dest->reg) = 1;
	}
	sprintf(assembly, "slti   %s,   %s,   0x%04x", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), op_src2->imm);
}

make_helper(sltiu) {
	decode_imm_type(instr);
	reg_w(op_dest->reg) = (op_src1->val < sign_extend32(op_src2->val))? 1:0;
	sprintf(assembly, "sltiu   %s,   %s,   0x%04x", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), op_src2->imm);
}

make_helper(andi) {
	decode_imm_type(instr);
	reg_w(op_dest->reg) = op_src1->val & op_src2->val;
	sprintf(assembly, "andi   %s,   %s,   0x%04x", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), op_src2->imm);
}

make_helper(ori) {
	decode_imm_type(instr);
	reg_w(op_dest->reg) = op_src1->val | op_src2->val;
	sprintf(assembly, "ori   %s,   %s,   0x%04x", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), op_src2->imm);
}

make_helper(xori) {
	decode_imm_type(instr);
	reg_w(op_dest->reg) = op_src1->val ^ op_src2->val;
	sprintf(assembly, "xori   %s,   %s,   0x%04x", REG_NAME(op_dest->reg), REG_NAME(op_src1->reg), op_src2->imm);
}

make_helper(lui) {

	decode_imm_type(instr);
	reg_w(op_dest->reg) = (op_src2->val << 16);
	sprintf(assembly, "lui   %s,   0x%04x", REG_NAME(op_dest->reg), op_src2->imm);
}

make_helper(lb) {

	decode_imm_type(instr);
	uint32_t addr = op_src1->val + sign_extend32(op_src2->val);
	uint32_t data = mem_read(addr,1);
	reg_w(op_dest->reg) = sign_extend32_8(data);
	sprintf(assembly, "lb   %s,   0x%01d(%s)", REG_NAME(op_dest->reg), op_src2->imm, REG_NAME(op_src1->reg));
}

make_helper(lbu) {

	decode_imm_type(instr);
	uint32_t addr = op_src1->val + sign_extend32(op_src2->val);
	reg_w(op_dest->reg) = mem_read(addr,1);
	sprintf(assembly, "lbu   %s,   0x%01d(%s)", REG_NAME(op_dest->reg), op_src2->imm, REG_NAME(op_src1->reg));
}

make_helper(lh) {//YY

	decode_imm_type(instr);
	uint32_t addr = op_src1->val + sign_extend32(op_src2->val);
	if(addr << 31 != 0){
		sprintf(assembly,"Exception:Address Error!");
		cpu.cpu0[8] = addr;
		SignalException(ADEL);
	}
	uint32_t data = mem_read(addr,2);
	reg_w(op_dest->reg) = sign_extend32(data);
	sprintf(assembly, "lh   %s,   0x%01d(%s)", REG_NAME(op_dest->reg), op_src2->imm, REG_NAME(op_src1->reg));
}

make_helper(lhu) {//YY

	decode_imm_type(instr);
	uint32_t addr = op_src1->val + sign_extend32(op_src2->val);
	if(addr << 31 != 0){
		sprintf(assembly,"Exception:Address Error!");
		cpu.cpu0[8] = addr;
		SignalException(ADEL);
	}
	reg_w(op_dest->reg) = mem_read(addr,2);
	sprintf(assembly, "lhu   %s,   0x%01d(%s)", REG_NAME(op_dest->reg), op_src2->imm, REG_NAME(op_src1->reg));
}

make_helper(lw) {//YY

	decode_imm_type(instr);
	uint32_t addr = op_src1->val + sign_extend32(op_src2->val);
	if(addr << 30 != 0){
		sprintf(assembly,"Exception:Address Error!");
		cpu.cpu0[8] = addr;
		SignalException(ADEL);
	}
	reg_w(op_dest->reg) = mem_read(addr,4);
	sprintf(assembly, "lw   %s,   0x%01d(%s)", REG_NAME(op_dest->reg), op_src2->imm, REG_NAME(op_src1->reg));
}

make_helper(sb) {

	decode_imm_type(instr);
	uint32_t addr = op_src1->val + sign_extend32(op_src2->val);
	uint32_t data = reg_w(op_dest->reg);
	mem_write(addr, 1, data);
	sprintf(assembly, "sb   %s,   0x%01d(%s)", REG_NAME(op_dest->reg), op_src2->imm, REG_NAME(op_src1->reg));
}

make_helper(sh) {//YY

	decode_imm_type(instr);
	uint32_t addr = op_src1->val + sign_extend32(op_src2->val);
	if(addr << 31 != 0){
		sprintf(assembly,"Exception:Address Error!");
		cpu.cpu0[8] = addr;
		SignalException(ADES);
	}
	uint32_t data = reg_w(op_dest->reg);
	mem_write(addr, 2, data);
	sprintf(assembly, "sb   %s,   0x%01d(%s)", REG_NAME(op_dest->reg), op_src2->imm, REG_NAME(op_src1->reg));
}

make_helper(sw) {//YY

	decode_imm_type(instr);
	uint32_t addr = op_src1->val + sign_extend32(op_src2->val);
	if(addr << 30 != 0){
		sprintf(assembly,"Exception:Address Error!");
		cpu.cpu0[8] = addr;
		SignalException(ADES);
	}
	uint32_t data = reg_w(op_dest->reg);
	mem_write(addr, 4, data);
	sprintf(assembly, "sw   %s,   0x%01d(%s)", REG_NAME(op_dest->reg), op_src2->imm, REG_NAME(op_src1->reg));
}

make_helper(beq) {

	decode_imm_type(instr);
	if(op_src1->val == reg_w(op_dest->reg)){
		cpu.pc = cpu.pc + 4 + sign_extend32(op_src2->val << 2);
	}
	else
		cpu.pc = cpu.pc + 4;
	sprintf(assembly, "beq   %s,   %s,   0x%04x", REG_NAME(op_src1->reg), REG_NAME(op_dest->reg), op_src2->imm);
}

make_helper(bne) {

	decode_imm_type(instr);
	if(op_src1->val != reg_w(op_dest->reg)){
		cpu.pc = cpu.pc + 4 + sign_extend32(op_src2->val << 2);
	}
	else
		cpu.pc = cpu.pc + 4;
	sprintf(assembly, "bne   %s,   %s,   0x%04x", REG_NAME(op_src1->reg), REG_NAME(op_dest->reg), op_src2->imm);
}

make_helper(bgtz) {//--

	decode_imm_type(instr);
	if(op_src1->val > 0){
		cpu.pc = cpu.pc + 4 + sign_extend32(op_src2->val << 2);
	}
	else
		cpu.pc = cpu.pc + 4;
	sprintf(assembly, "bgtz   %s,   0x%04x", REG_NAME(op_src1->reg), op_src2->imm);
}

make_helper(blez) {

	decode_imm_type(instr);
	if(op_src1->val <= 0){
		cpu.pc = cpu.pc + 4 + sign_extend32(op_src2->val << 2);
	}
	else
		cpu.pc = cpu.pc + 4;
	sprintf(assembly, "blez   %s,   0x%04x", REG_NAME(op_src1->reg), op_src2->imm);
}

make_helper(bltz) {
	decode_imm_type(instr);
	if(op_src1->val < 0){
		cpu.pc = cpu.pc + 4 + sign_extend32(op_src2->val << 2);
	}
	else
		cpu.pc = cpu.pc + 4;
	sprintf(assembly, "bltz   %s,   0x%04x", REG_NAME(op_src1->reg), op_src2->imm);
	
}

make_helper(bgez){
	decode_imm_type(instr);
	if(op_src1->val >= 0){
			cpu.pc = cpu.pc + 4 + sign_extend32(op_src2->val << 2);
		}
	else
		cpu.pc = cpu.pc + 4;
	sprintf(assembly, "bgez   %s,   0x%04x", REG_NAME(op_src1->reg), op_src2->imm);
}

make_helper(bltzal){
	decode_imm_type(instr);
	reg_w(31) = cpu.pc + 8;
	if(op_src1->val < 0){
		cpu.pc = cpu.pc + 4 + sign_extend32(op_src2->val << 2);
	}
	else
		cpu.pc = cpu.pc + 4;
	sprintf(assembly, "bltzal   %s,   0x%04x", REG_NAME(op_src1->reg), op_src2->imm);
}

make_helper(bgezal){
	decode_imm_type(instr);
	reg_w(31) = cpu.pc + 8;
	if(op_src1->val >= 0){
		cpu.pc = cpu.pc + 4 + sign_extend32(op_src2->val << 2);
	}
	else
		cpu.pc = cpu.pc + 4;
	sprintf(assembly, "bgezal   %s,   0x%04x", REG_NAME(op_src1->reg), op_src2->imm);
}