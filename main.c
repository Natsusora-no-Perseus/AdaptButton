#include <boards/pico.h>
#include <hardware/pwm.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "serialsh.h"
#include "sfuncs.h"


#define CMD_BUFFER_LEN 64

int pico_led_init(void) {
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return 0;
}

int pico_pwm_init(void) {
    gpio_set_function(PWM_CHAN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(PWM_CHAN);
    pwm_set_wrap(slice_num, 3);
    pwm_set_gpio_level(PWM_CHAN, 1);
    pwm_set_enabled(slice_num, false);
    return 0;
}

int main() {
    stdio_init_all(); // Initiate serial over USB

    pico_led_init();

    pico_pwm_init();

    adc_init();
    adc_gpio_init(28); // ADC2 = GPIO28
    adc_select_input(2);

    printf("Pico ADC2 Reader Initialized!\n");

    // Register custom functions:
    const char echo_args_name[] = "eargs";
    srsh_register_func((void *)echo_args, echo_args_name);
    const char pwm_args_name[] = "pwm";
    srsh_register_func((void *)pwm_arg_parse, pwm_args_name);
    const char adc_args_name[] = "adc";
    srsh_register_func((void *)adc_arg_parse, adc_args_name);

    // uint8_t blink_flag = 0;

    char cmd_buf[CMD_BUFFER_LEN];

    while (true) {
        memset(cmd_buf, 0, CMD_BUFFER_LEN);
        int idx = 0;

        while (idx < CMD_BUFFER_LEN-1) {
            char c = getchar();
            if (c == '\n') {
                break;
            }
            cmd_buf[idx++] = c;
        }

        if (idx > 0 && cmd_buf[idx - 1] == '\n') {
            cmd_buf[idx-1] = '\0';
        } else {
            cmd_buf[idx] = '\0';
        }
        printf("Received: %s\n", cmd_buf);


        int call_ret = srsh_parse(cmd_buf);
        printf("Parse result: %d\n", call_ret);

    }

    return 0;
}
