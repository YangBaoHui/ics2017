#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,TK_NotEqual,TK_ShiftLeft,TK_ShiftRight,TK_GreaterOrEqual,TK_SmallerOrEqual,TK_Number,TK_Number16,TK_And,TK_Or,TK_Register,TK_Variable,TK_Minus,TK_Deref

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
  int association;
  int precedence;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */ 

  {" +", TK_NOTYPE,0,100},    // spaces
  {"\\+", '+',1,12},         // plus
  {"==", TK_EQ,1,10},         // equal
  {"!=",TK_NotEqual,1,10},
  {"-",'-',1,12},            //substract
  {"\\*",'*',1,13},          //multiply
  {"/",'/',1,13},            //divide
  {"%",'%',1,13},            //mod
  {"\\(",'(',1,15},
  {"\\)",')',1,15},
  {"<<",TK_ShiftLeft,1,11},
  {">>",TK_ShiftRight,1,11},
  {">=",TK_GreaterOrEqual,1,10},
  {"<=",TK_SmallerOrEqual,1,10},
  {">",'>',1,10},
  {"<",'<',1,10},
  {"\\!",'!',1,14},
  {"\\$[a-zA-Z]+",TK_Register,0,100},
  {"[0-9]+",TK_Number,0,100},
  {"0x[0-9a-fA-F]+",TK_Number16,0,100},
  {"[a-zA-Z_]+[a-zA-Z0-9_]*",TK_Variable,0,100},

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
  bool association;
  int precedence;
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

   while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          default: TODO();
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}


bool flag = true;

bool check_parentheses(int p, int q){
	    int count = 0;
		bool flag = true;
		if(tokens[p].type != '(' || tokens[q].type != ')')
			return false;
		int i;
		for(count = 0, i = p+1; i <= q-1; ++i){
			int tmp = tokens[i].type;
			if( tmp == '(' )
				count++;
			else if( tmp == ')' )
				count--;
			if (count < 0) 
				flag = false;
		}
		if(count != 0) flag = false;
		return flag;
}

int get_var(char*);

int eval(int p, int q, bool *success){
if(p>q){
	printf("Bad expression : p > q\n");
	*success = false;
	return 1;
	/*Bad expression*/
}
else if(p == q){ 
	/*Single token.
	 * * For now this token should be a number. 
	 * * Return the value of the number.
	 * */ 
	if(tokens[p].type == TK_Number){
		int number = 0;
		char *tmp = tokens[p].str;
	 	while(*tmp){
			number = number * 10 + *tmp - '0';
			tmp++;
		}
		return number;
	}
	else if(tokens[p].type == TK_Number16){
		int number = 0;
		char *tmp = tokens[p].str + 2;
		while(*tmp){
			number = number * 16;
			if(*tmp >= '0' && *tmp <= '9')
				number += *tmp - '0';
			else if(*tmp >= 'a' && *tmp <= 'f')
				number += *tmp - 'a' + 10;
			else if(*tmp >= 'A' && *tmp <= 'F')
				number += *tmp - 'A' + 10;
			else{
				printf("Error occured when recognizing TK_Number16\n");
				*success = false;
				return 1;
		 	}
			tmp++;
		 }
		return number;
	}

/*	else if(tokens[p].type == TK_Register){
		char *reg = tokens[p].str;
		if( strlen(reg) <= 2 ){
			printf("Wrong register name! %s\n",reg);
			*success = false;
			return 1;
		}
		reg ++;
	    //Log("reg = %s\n",reg);
		int i = 0;
		if(strcasecmp(reg, "eip") == 0){
			return cpu.eip;
		}
		if(strcasecmp(reg, "cf") == 0){
			return cpu.CF;
     	}
		if(strcasecmp(r eg, "pf") == 0){
			return cpu.PF;
		}
		if(strcasecmp(reg, "zf") == 0){
			return cpu.ZF;
		} 
		if(strcasecmp(reg, "sf") == 0){
			return cpu.SF;
		} 
		if(strcasecmp(reg, "if") == 0){
			return cpu.IF;
		} 
		if(strcasecmp(reg, "df") == 0){
			return cpu.DF;
		} 
		if(strcasecmp(reg, "of") == 0){
			return cpu.OF;
		} 
		for(i = 0; i < 8; ++i)
			if(strcasecmp(regsl[i], reg) == 0)
				return reg_l(i);
		for(i = 0; i < 8; ++i)
			if(strcasecmp(regsw[i], reg) == 0)
				return reg_w(i);
		for(i = 0; i < 8; ++i)
			if(strcasecmp(regsb[i], reg) == 0)
				return reg_b(i);
		printf("didn't find register : %s\n", tokens[p].str);
		*success = false;
		return 1;
	}
	else if(tokens[p].type == TK_Variable){
		int result = get_var(tokens[p].str);
		if(result == -1){
			*success = false;
			printf("didn't find variable : %s\n", tokens[p].str);
			return 1;
		}
		else return result;
	}
*/
}
else if(check_parentheses(p, q) == true) {
	/* The expression is surrounded by a matched pair of parentheses. 
	 * * If that is the case, just throw away the parentheses.
 	 * */
	return eval(p + 1, q - 1,success); 
} 
else{
	bool par_err = false; 
	int op = p;
	int count = 0;
	int i = 0;
	for(i = p; i <= q; ++i){
		if(tokens[i].type == '(') count++;
		else if(tokens[i].type == ')') count--;
		else if(count == 0){
			if(tokens[i].precedence < tokens[op].precedence)
				op = i; 
			if(tokens[i].precedence == tokens[op].precedence && tokens[i].association == 1)
				op = i; 
 		}
		if(count < 0) par_err = true;
 	}
	if(count != 0) par_err = true;
 	if(par_err){
		printf("parentheses error!\n");
		*success = false;
		return 1;
	}
	int op_type = tokens[op].type;
	int val1 = 0, val2 = 0;
	if(op_type == TK_Minus || op_type ==TK_Deref || op_type == '!'){
		val1 = eval(op+1, q, success);
 	}
	else{
		val1 = eval(p, op- 1,success);
		val2 = eval(op + 1, q,success); 
 	}
 	switch(op_type) {
		case '+': return val1 + val2;
		case '-': return val1 - val2;
		case '*': return val1 * val2;
		case '/': 
			  if(val2==0){
				  printf("Error! divided by 0\n");
				  *success = false;
				  return 1;
 			  }
			  else return val1/val2;
		case '%':
 			  if(val2==0){
				  printf("Error! mod by 0\n");
				  *success = false;
				  return 1;
			  }
			  else return val1 % val2;
		case TK_Minus: return -val1;
		case TK_Deref: return vaddr_read(val1,4);
		case '!' : return !val1;
		case TK_And : return val1 && val2;
		case TK_Or: return val1 || val2;
		case TK_EQ: return val1 == val2;
		case TK_NotEqual: return val1 != val2;
		case '>' : return val1 > val2;
		case '<' : return val1 < val2;
		case TK_ShiftLeft: return val1 << val2;
		case TK_ShiftRight: return val1 >> val2;
		case TK_GreaterOrEqual: return val1 >= val2;
		case TK_SmallerOrEqual: return val1 <= val2;
		default: printf("op_type : didn't find %d\n",op_type);
			 *success = false;
			 return 1;
	}
}
*success = false;
return 1;
} 

int expr(char *e, bool *success) {
 if( strlen(e) > 31 ){
	 *success = false;
	printf("Too long expression!\n");
   return 0;
   }   
    if (!make_token(e)) {
    *success = false;
    return 0;
   }

  /* TODO: Insert codes to evaluate the expression. */

  int i = 0;
  for(i = 0; i < nr_token; i++){
	   if(tokens[i].type == '-' && (i == 0 || (tokens[i-1].type !=TK_Number && tokens[i-1].type != TK_Register))){
		  tokens[i].type = TK_Minus;
		  tokens[i].precedence = 14;
  tokens[i].association = 0;
	  }
	  if(tokens[i].type == '*' && (i == 0 || (tokens[i-1].type !=TK_Number && tokens[i-1].type !=TK_Register))){
		  tokens[i].type = TK_Deref;
		  tokens[i].precedence = 14;
 		  tokens[i].association = 0;
	   }
  } 
  *success = true;
  return eval(0,nr_token-1,success);
  //panic("please implement me");
  return 0;
}
