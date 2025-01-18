#include <string.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include "hardware/pwm.h"

#include "sfuncs.h"

int echo_back(char *args) {
    if (args == NULL) {
        printf("args is NULL\n");
        return 1;
    }
    printf("%s\n", args);
    return 0;
}

const char FECHO_ARGS_NAME[] = "eargs";

int echo_args(char *args) {
    if (args == NULL) {
        printf("args is NULL\n");
        return 1;
    }
    char *params = strtok(args, " ");
    while (params != NULL)
    {
        if (params[0] == '-') {
            printf("arg %s; ", params);
        }
        params = strtok(NULL, " ");
    }
    return 0;
}

const int PWM_CHAN = 21;
int set_pwm(char *args) {
    if (args == NULL || *args == '\0') {
        printf("PWM: No args.\n");
        return 1;
    }

    char *params = strtok(args, " ");
    while (params != NULL)
    {
        if (params[0] == '-') {
            switch (param[1])
            {
                case 'w': // Set wrap
            
                    
            printf("arg %s; ", params);
        } else {
            printf("opt %s; ", params);
        }
    }
    return 0;
}

