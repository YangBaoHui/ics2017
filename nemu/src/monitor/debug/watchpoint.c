#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

void new_wp(char* args){
	if( free_ == NULL ) assert(0);
	WP* tmp = free_;
	strcpy(tmp->s_expr, args);
	bool success;
	tmp->oldvalue = expr(args, &success);
	if( success == false ){
		printf("Failed to create a new watchpoint(bad expression)\n");
		return;
	}
	free_ = free_ -> next;
	tmp -> next = head;
	head = tmp;
	return;
}

void free_wp(int number){
	WP* tmp = head;
	WP* last = head;
	while(tmp){
		if( tmp -> NO == number)
			break;
		last = tmp;
		tmp = tmp -> next;
	}
	if( tmp == NULL ){
		printf("didn't find watchpoint number : %d\n", number);
		return;
	}
	if( tmp == head ) 
		head = tmp -> next;
	else
		last -> next = tmp -> next;
	tmp -> next = free_;
	free_ = tmp;
	printf("watchpoint %d deleted\n", number);
	return;
}

void check_wp(int* nemu_state){
	WP* wp = head;
	while( wp ){
		bool success;
		int value = expr(wp->s_expr, &success);
		if( value != wp->oldvalue ){
			*nemu_state = 0;
			printf("%8x hit watchpoint:the value of the ( %s ) changed from 0x%x to 0x%x\n", wp->NO,wp->s_expr, wp->oldvalue, value);
			wp -> oldvalue = value;
		}
		wp = wp -> next;
	}
}

void info_wp(){
	if( head == NULL ){
		printf("There is no watchpoint!\n");
		return;
	}
	printf("watchpoints:\nNO\tEXPR\t\tVALUE\n");
	int i = 0;
	for(i = 0; i < 32; ++i){
		WP* wp = head;
		while( wp && wp->NO != i )
			wp = wp->next;
		if(wp)
			printf("%d\t%s\t0x%x\t\n", wp->NO, wp->s_expr, wp->oldvalue);
	}
	return;
}




