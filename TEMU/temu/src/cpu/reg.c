#include "temu.h"
#include <stdlib.h>

CPU_state cpu;

const char *regfile[] = {"$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};
const char *cpu0file[] = {"index", "random", "entrylo0", "entrylo1", "contet", "pagemask", "wired", "reserved0", "badvaddr", "count", "entryhi", "compare", "status", "cause", "epc", "prid", "config", "lladdr", "watchlo", "watchhi", "reserved1", "reserved2", "reserved3", "debug", "depc", "reserved4", "errctl", "reserved5", "taglo_datalo", "reserved6", "errorepc", "desave"};
void display_reg() {
        int i;
        for(i = 0; i < 32; i ++) {
                printf("%s\t\t0x%08x\t\t%d\n", regfile[i], cpu.gpr[i]._32, cpu.gpr[i]._32);
        }

        printf("%s\t\t0x%08x\t\t%d\n", "$pc", cpu.pc, cpu.pc);
		printf("%s\t\t0x%08x\t\t%d\n", "$hi", cpu.hi, cpu.hi);
		printf("%s\t\t0x%08x\t\t%d\n", "$lo", cpu.lo, cpu.lo);

		for(i = 0; i < 32; i ++) {
                printf("$%s\t\t0x%08x\t\t%d\n", cpu0file[i], cpu.cpu0[i], cpu.cpu0[i]);
        }
}

uint32_t get_reg_val(const char *s, bool *success) {
	int i;
	*success = true;
	for(i = 0; i < 32; i ++) {
		if(strcmp(regfile[i], s) == 0) {
			return reg_w(i);
		}
	}

	*success = false;
	return 0;
}