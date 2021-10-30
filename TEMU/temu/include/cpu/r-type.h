#ifndef __RTYPE_H__
#define __RTYPE_H__
//14
make_helper(add);
make_helper(addu);
make_helper(sub);
make_helper(subu);
make_helper(slt);
make_helper(sltu);
make_helper(and);
make_helper(or);
make_helper(nor);
make_helper(xor);
make_helper(mult);
make_helper(multu);
make_helper(div);
make_helper(divu);
//6
make_helper(sll);
make_helper(srl);
make_helper(sra);
make_helper(sllv);
make_helper(srlv);
make_helper(srav);
//4
make_helper(mfhi);
make_helper(mflo);
make_helper(mthi);
make_helper(mtlo);
//2
make_helper(jr);
make_helper(jalr);
//2
make_helper(syscall);
make_helper(_break);
#endif

uint32_t sign_extend32(uint32_t imm);
uint32_t sign_extend32_8(uint32_t imm);