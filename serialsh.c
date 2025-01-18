#include "serialsh.h"

static SrshFuncEntry srsh_func_list[SRSH_MAX_FUNCS];
static int srsh_func_cnt = 0;

int srsh_parse(char* input_b, int cut_trailing) {
    /**
     * @param input_b: Pointer to buffer containing null-terminated input.
     * @param cut_trailing: Set TRUE if input contains '\n' before null terminator.
     * @return: 0 on success, else on error.
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

int srsh_register_func(void *func, char *name, SrshArgEntry *args) {
    /**
     * srsh_register_func
     * Registers a function to a internal list.
     * @param func: Function pointer to the called function.
     * @param name: Name of the function being registered.
     * @param args: List of argument entries to be matched and called.
     * @return: 0 on success; else on error.
     * @warning: Make sure your registered function handles NULL as args correctly!
     */
    if (srsh_func_cnt >= SRSH_MAX_FUNCS) {
        return 1;
    }
    srsh_func_list[srsh_func_cnt].name = name;
    srsh_func_list[srsh_func_cnt].func = func;
    srsh_func_list[srsh_func_cnt].args = args;
    srsh_func_cnt ++;
    return 0;
}

int srsh_call_func(char *name, char *args) {
    /**
     * srsh_call_func
     * Finds `name` in the registered list and calls it.
     * @param name: Name of the function being called.
     * @param args: Pointer to all arguments.
     * @return: Called function's return value; -1 if no matching function found.
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

int srsh_list_func(void) { // TODO: implement this
    return 1;
}

int srsh_proc_args(const char *input, const SrshArgEntry *arglst, size_t argcnt) {
    /**
     * srsh_proc_args
     * Separates arguments and call their respective routines.
     * @param input: String given by `srsh_call_func`.
     * @param arglst: Array of `SrshArgEntry` specifying args and their params.
     * @param argcnt: Length of arglst.
     */
    const char *iptr = input; // Pointer to traverse input
    
    if (arglst == NULL || argcnt == 0) {
        return 1; // No arguments to parse against
    }

    if (input == NULL) {
        return 1;
    }

    while (*iptr) {
        while (isspace((unsigned char)*iptr)) {
            iptr ++; // Move past whitespaces
        }

        if (*iptr == '-') {
            const char arg = *(++iptr); // Skip the '-'

            // Try to match arg against arglst
            const SrshArgEntry *matched_arg = NULL;
            iptr ++;
            for (size_t i=0; i < argcnt; i++) {
                if (arglst[i].name == arg) {
                    matched_arg = &arglst[i];
                    break;
                }
            }
            if (matched_arg == NULL) {
                return 1; // No matching arg found
            }

            // Separate parameters
            const char *params[SRSH_MAX_ARGS] = {0};
            for (int i=0; i < matched_arg->num_params; i++) {
                while (isspace((unsigned char) *iptr)) {
                    iptr++; // Move past whitespaces
                }
                if (*iptr == '\0') {
                    return 1; // In this loop num_params > 0
                }
                const char *param_start = iptr;
                while (*iptr && !isspace((unsigned char) *iptr)) {
                    iptr ++;
                }
                size_t param_len = iptr - param_start;
                char *param = malloc(param_len + 1);
                if (!param) {
                    return 1; // Malloc failed
                }
                strncpy(param, param_start, param_len);
                param[param_len] = '\0';
                params[i] = param;
            }

            // Call process with params
            int result = matched_arg->process(params); 

            // Clean up
            for (int i=0; i< matched_arg->num_params; i++) {
                free((void *)params[i]); // Free memory
            }

            return result;
        } else {
            return 1; // Invalid format
        }
    }
    return 0;
}
