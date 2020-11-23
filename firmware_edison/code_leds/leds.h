#ifndef LEDS_H_
#define LEDS_H_

#include <pwm.h>

// CONFIGURACOES INICIAIS DOS PINOS PWM DA EDISON
int init_leds();

void set_leds_off();

void set_led_red();

void set_led_green();

void set_led_yellow();

#endif
