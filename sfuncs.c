#include <pico/time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/timer.h"

#include "cJSON.h"

#include "sfuncs.h"
#include "serialsh.h"

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

// ====== PWM config =====

int proc_pwm_wrap(const char **params) {
    uint16_t param = atoi(params[0]);
    if (param < 1) {
        pwm_set_wrap(pwm_gpio_to_slice_num(PWM_CHAN), 3);
        printf("Invalid PWM wrap val; default to 3.\n");
    } else {
        pwm_set_wrap(pwm_gpio_to_slice_num(PWM_CHAN), param);
        printf("PWM wrap set to %d.\n", param);
    }
    return 0;
}

int proc_pwm_level(const char **params) {
    uint16_t param = atoi(params[0]);
    pwm_set_gpio_level(PWM_CHAN, param);
    printf("PWM level set to %d.\n", param);
    return 0;
}

int proc_pwm_state(const char **params) {
    uint16_t param = atoi(params[0]);
    pwm_set_enabled(pwm_gpio_to_slice_num(PWM_CHAN), param);
    printf("PWM switched to %d.\n", param);
    return 0;
}

int proc_pwm_help(const char **params) {
    (void)params; // Do nothing; prevent -Wunused
    printf("PWM control: '-w|l|s|h' <param>\n");
    return 0;
}

int pwm_arg_parse(char *args) {
    SrshArgEntry arg_lst[] = {
        {'w', 1, proc_pwm_wrap},
        {'l', 1, proc_pwm_level},
        {'s', 1, proc_pwm_state},
        {'h', 0, proc_pwm_help}
    };
    int ret = srsh_proc_args(args, arg_lst, sizeof(arg_lst)/sizeof(arg_lst[0]));
    return ret;
}

// ====== ADC config =====

enum adc_print_fmt {ADC_NORM, ADC_JSON};
volatile bool adc_running = false;
volatile unsigned int adc_smpl_intv = 100; // ADC sampling interval (ms)
volatile enum adc_print_fmt adc_format = ADC_NORM;
const unsigned int adc_gpio = 28; // ADC pin

bool adc_timer_callback(struct repeating_timer *t) {
    if (adc_running) {
        adc_select_input(adc_gpio-26);
        uint16_t result = adc_read();
        if (adc_format == ADC_JSON) {
            // Print in JSON
            uint32_t time_since_boot = to_ms_since_boot(get_absolute_time());
            cJSON *json = cJSON_CreateObject();
            cJSON_AddNumberToObject(json, "time", time_since_boot);
            cJSON_AddNumberToObject(json, "ADC", result);

            char *json_string = cJSON_PrintUnformatted(json);
            printf("%s\n", json_string);
            cJSON_Delete(json);
            cJSON_free(json_string);
        } else {
            printf("ADC value: %u\n", result);
        }
    }
    return true; // Continue repeating
}

int adc_read_continuous(int adc_gpio, int state, int interval) {
    if (adc_gpio < 26 || adc_gpio > 28) {
        return 1; // Invalid pin number
    }

    if (state) {
        if (!adc_running) {
            adc_smpl_intv = interval > 0 ? interval : 100; // Default to 100ms
            adc_running = true;

            static struct repeating_timer adc_timer;
            add_repeating_timer_ms(-adc_smpl_intv, adc_timer_callback,
                    NULL, &adc_timer);
            printf("ADC continuous sampling started; intv=%d\n", adc_smpl_intv);
        } else {
            printf("ADC sampling already running.\n");
        }
    } else {
        adc_running = false;
        printf("ADC continuous sampling stopped.\n");
    }
    return 0;
}

int proc_adc_state(const char **params) {
    int param = atoi(params[0]);
    if (param >= 0) {
        // Can't directly write adc_running here because adc_read_continuous
        // relies on its previous state to detect if it's already running
        adc_read_continuous(adc_gpio, param, adc_smpl_intv);
        printf("ADC state set to %d\n", adc_running);
    } else {
        printf("Invalid ADC state value.\n");
    }
    return 0;
}

int proc_adc_interval(const char **params) {
    int param = atoi(params[0]);
    if (param > 0) {
        adc_smpl_intv = atoi(params[0]);
    }
    adc_read_continuous(adc_gpio, adc_running, adc_smpl_intv);
    printf("ADC interval set to %d\n", adc_smpl_intv);
    return 0;
}

int proc_adc_format(const char **params) {
    int param = atoi(params[0]);
    if (param == ADC_NORM) {
        adc_format = ADC_NORM;
        printf("ADC print format set to NORM");
    } else if (param == ADC_JSON) {
        adc_format = ADC_JSON;
        printf("ADC print format set to JSON");
    } else {
        printf("Invalid ADC format value %d\n", param);
    }
    return 0;
}

int adc_arg_parse(char *args) {
    SrshArgEntry arg_lst[] = {
        {'s', 1, proc_adc_state},
        {'i', 1, proc_adc_interval},
        {'f', 1, proc_adc_format}
    };
    int ret = srsh_proc_args(args, arg_lst, sizeof(arg_lst)/sizeof(arg_lst[0]));
    return ret;
}
