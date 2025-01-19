#ifndef SFUNCS_H
#define SFUNCS_H

#define PWM_CHAN 21

#include <pico/time.h> // For struct repeating_timer

int echo_back(char *args);
int echo_args(char *args);
int proc_pwm_wrap(const char **params);
int proc_pwm_level(const char **params);
int proc_pwm_help(const char **params);
int pwm_arg_parse(char *args);

bool adc_timer_callback(struct repeating_timer *t);
int adc_read_continuous(int adc_gpio, int state, int interval);
int proc_adc_state(const char **params);
int proc_adc_interval(const char **params);
int adc_arg_parse(char *args);
#endif
