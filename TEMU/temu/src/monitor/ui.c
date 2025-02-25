#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "temu.h"

#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

void display_reg();

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(temu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args) {
	char *arg = strtok(NULL, " ");
	int i = 1;

	if(arg != NULL) {
		sscanf(arg, "%d", &i);
	}
	cpu_exec(i);
	return 0;
}

static int cmd_info(char *args) {
	char *arg = strtok(NULL, " ");

	if(arg != NULL) {
		if(strcmp(arg, "r") == 0) {
			display_reg();
		}
		else if(strcmp(arg, "w") == 0) {
			list_watchpoint();
		}
	}
	return 0;
}

static int cmd_x(char *args){  
    char *N = strtok(NULL," ");  
    char *EXPR = strtok(NULL," ");  
    int len;  
    uint32_t address;  
      
    sscanf(N, "%d", &len);  
    sscanf(EXPR, "%x", &address);  
      
    printf("0x%x:",address);  
    int i;
    for(i = 0; i < len; i ++){  
        printf("%08x ",mem_read(address,4));  
        address += 4;  
    }  
    printf("\n");  
    return 0;  
}

static int cmd_p(char *args) {
	bool success;

	if(args) {
		uint32_t r = expr(args, &success);
		if(success) { printf("0x%08x(%d)\n", r, r); }
		else { printf("Bad expression\n"); }
	}
	return 0;
}

static int cmd_w(char *args) {
	if(args) {
		int NO = set_watchpoint(args);
		if(NO != -1) { printf("Set watchpoint #%d\n", NO); }
		else { printf("Bad expression\n"); }
	}
	return 0;
}

static int cmd_d(char *args) {
	int NO;
	sscanf(args, "%d", &NO);
	if(!delete_watchpoint(NO)) {
		printf("Watchpoint #%d does not exist\n", NO);
	}

	return 0;
}

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit TEMU", cmd_q },

	/* TODO: Add more commands */

	{ "si", "Single step", cmd_si },
	{ "info", "info r - print register values; info w - show watch point state", cmd_info },
	{ "x", "Examine memory", cmd_x },
    { "p", "Evaluate the value of expression", cmd_p },
	{ "w", "Set watchpoint", cmd_w },
	{ "d", "Delete watchpoint", cmd_d }

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
