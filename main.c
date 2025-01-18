#include <boards/pico.h>
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

int main() {
    stdio_init_all(); // Initiate serial over USB

    pico_led_init();

    adc_init();

    adc_gpio_init(28); // ADC2 = GPIO28

    adc_select_input(2);

    printf("Pico ADC2 Reader Initialized!\n");

    // Register custom functions:
    char echo_args_name[] = "eargs";
    srsh_register_func((void *)echo_args, echo_args_name, NULL);

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

        /*
        uint16_t raw_adc = adc_read();

        printf("Raw value: %u, ", raw_adc);
        printf("calculated val: %f\n", raw_adc * 3.3 / 4096);

        if (blink_flag != 0) {
            gpio_put(PICO_DEFAULT_LED_PIN, true);
            blink_flag = 0;
        } else {
            gpio_put(PICO_DEFAULT_LED_PIN, false);
            blink_flag = 1;
        }

        sleep_ms(300);
        */

        int call_ret = srsh_parse(cmd_buf, 0);
        printf("Parse result: %d\n", call_ret);

    }

    return 0;
}
