#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

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

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);

static int cmd_p(char *args);

static int cmd_w(char *args);

static int cmd_d(char *args);


static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q},
  { "si", "Go on some steps", cmd_si },
  {"info", "Print register states or watchpoint", cmd_info },
  {"x", "Scan memory", cmd_x},
  {"p","Calculate the expr",cmd_p},
  {"w","set watchpoint",cmd_w},
  {"d","delete watchpoint",cmd_d},
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0;i<NR_CMD;i++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char *args) {
	char *arg = strtok(NULL, " ");
        int n = 0;
	if(arg==NULL) cpu_exec(1);
	else
	{
        sscanf(arg, "%d", &n);
	if(n<=20){
        for (int j = 0; j < n; ++j) {
         cpu_exec(1);
		  }
	}
	else printf("Input Error");
	}
	    return 1;
}

void print_reg() {
	    for (int i = 0; i < 8; ++i) {
                printf("%s %x\n", regsl[i], cpu.gpr[i]._32);  }
	    for (int i = 0; i < 8; ++i) {
	        printf("%s %x\n", regsw[i], cpu.gpr[i]._16);				    }
            for (int i = 0; i < 8; ++i) {
	           for (int j = 0; j < 2; j++)
          printf("%s %x\n", regsb[i], cpu.gpr[i]._8[j]); 
		 }
}

static int cmd_info(char *args) {
	    char *arg = strtok(NULL, " ");
	        if (strcmp(arg, "r") == 0)  print_reg();
			else if(strcmp(arg, "w")==0) info_wp();
	        else printf("Unknown command");
		 	return 1;
}

static int cmd_x(char *args) {

	    char *arg1=strtok(NULL," ");
	    char *arg2=strtok(NULL," ");

	     int len;
	    vaddr_t addr;
	    sscanf(arg1,"%d",&len);
            sscanf(arg2,"%x",&addr);	
	    printf("0x%x:",addr);
            for (int j = 0; j <len ; ++j) {
	        printf("%x ",vaddr_read(addr,4));
		        addr+=4;
		   }	
	    printf("\n");
        return 1;
}

static int cmd_p(char *args){
	if(args == NULL){
		printf("Please input an expression!\n");
		return 0;
	}
	bool success;
	int ans = expr(args, &success);
	if(!success)
		printf("There is an error occured in function expr()\n");
	else 
		printf("The result is:%d\t(0x%x)\n",ans,ans);
	return 0;
}

static int cmd_w(char *args){
	    if(args == NULL)
			        printf("please enter a expr, so you can watch it\n");
		    else{
				        new_wp(args);
						    }   
			    
			    return 0;
}

static int cmd_d(char *args){
	if(args == NULL){
		printf("Please input a number!\n");
		return 0;
	}
	int num;
	if( sscanf(args, "%d", &num) == 0 ){
		printf("Not a number!\n");
		return 0;
	}
	if( num < 0 || num >= 32 ){
		printf("n must be smaller than 32 and no smaller than 0\n");
		return 0;
	}
	free_wp(num);
	return 0;
}



void ui_mainloop(int is_batch_mode) {
	  if (is_batch_mode) {
		      cmd_c(NULL);
		          return;
			    }


  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
