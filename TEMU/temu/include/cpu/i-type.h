#ifndef __ITYPE_H__
#define __ITYPE_H__

//8
make_helper(addi);
make_helper(addiu);
make_helper(slti);
make_helper(sltiu);
make_helper(andi);
make_helper(ori);
make_helper(xori);
make_helper(lui);
//8
make_helper(lb);
make_helper(lbu);
make_helper(lh);
make_helper(lhu);
make_helper(lw);
make_helper(sb);
make_helper(sh);
make_helper(sw);
//8
make_helper(beq);
make_helper(bne);
make_helper(bgtz);
make_helper(blez);
make_helper(bltz);
make_helper(bgez);
make_helper(bltzal);
make_helper(bgezal);
#endif
uint32_t sign_extend32(uint32_t imm);
uint32_t sign_extend32_8(uint32_t imm);

void status_exl_update(int a);