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
	tmp->last_value = expr(args, &success);
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


