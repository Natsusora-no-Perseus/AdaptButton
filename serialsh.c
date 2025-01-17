#include "serialsh.h"
#include <string.h>

typedef struct {
    char *name;
    void *func;
} SrshFuncEntry;

static SrshFuncEntry srsh_func_list[SRSH_FUNCLST_LEN];
static int srsh_func_cnt = 0;

int srsh_parse(char* input_b, int cut_trailing) {
    /**
     * @param Pointer to buffer containing null-terminated input.
     * @param Set TRUE if input contains '\n' before terminating '\0'.
     * @return 0 on success, else on error.
     */

    // Remove the trailing '\n' in input:
    if (cut_trailing) {
        input_b[strcspn(input_b, "\n")] = 0;
    }

    if (input_b == NULL || input_b[0] == '\0') {
        // Quit if buffer is NULL or contains nothing
        return 1;
    }

    // Do what strtok is supposed to do, because using it here produce unwanted
    // side effects
    char *cmd = input_b;
    char *params = strchr(input_b, ' ');

    if (params != NULL) {
        *params = '\0';
        params++;
    }

    if (cmd == NULL) {
        return 1;
    }

    srsh_call_func(cmd, params);

    return 0;
}

int srsh_register_func(void *func, char *name) {
    /**
     * Registers a function to a internal list.
     * @param Function pointer to the called function.
     * @param Name of the function being registered.
     * @return 0 on success; else on error.
     * @warning Make sure your registered function handles NULL as args correctly!
     */
    if (srsh_func_cnt >= SRSH_FUNCLST_LEN) {
        return 1;
    }
    srsh_func_list[srsh_func_cnt].name = name;
    srsh_func_list[srsh_func_cnt].func = func;
    srsh_func_cnt ++;
    return 0;
}
    
int srsh_call_func(char *name, char *args) {
    /**
     * Finds `name` in the registered list and calls it.
     * @param Name of the function being called.
     * @param Pointer to all arguments.
     * @returns Called function's return value; -1 if no matching function found.
     */
    int idx = 0;
    while (idx < srsh_func_cnt) {
        if (strcmp(name, srsh_func_list[idx].name) == 0) {
            int (*func)(char*) = (int (*)(char*))srsh_func_list[idx].func;
            return func(args);
        } else {
            idx ++;
        }
    }
    return -1;
}

int srsh_list_func(void);
