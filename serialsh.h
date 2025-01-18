#ifndef SERIALSH_H
#define SERIALSH_H

#define SRSH_MAX_FUNCS 16 // Max number of functions that can be registered
#define SRSH_MAX_ARGS 8 // Max number of arguments for each function

#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

typedef struct {
    const char name; // Name (e.g. "-w" -> 'w')
    int num_params; // Number of expected parameters
    int (*process)(const char **params);
} SrshArgEntry;

typedef struct {
    const char *name;
    void *func;
    SrshArgEntry *args;
} SrshFuncEntry;

int srsh_parse(char* input, int cut_trailing);
int srsh_register_func(void *func, char *name, SrshArgEntry *args);
int srsh_call_func(char *name, char *args); 
int srsh_list_func(void);
int srsh_proc_args(const char *input, const SrshArgEntry *arglst, size_t argcnt);

#endif
