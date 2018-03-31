#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char s_expr[64];
  int oldvalue;
  /* TODO: Add more members if necessary */


} WP;
void new_wp(char *args);
void free_wp(int number);
void check_wp();
void info_wp();
#endif
