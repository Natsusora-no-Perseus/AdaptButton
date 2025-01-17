#ifndef SERIALSH_H
#define SERIALSH_H

#define SRSH_FUNCLST_LEN 16

int srsh_parse(char* input, int cut_trailing);
int srsh_register_func(void *func, char *name);
int srsh_call_func(char *name, char *args); 
int srsh_list_func(void);

#endif
