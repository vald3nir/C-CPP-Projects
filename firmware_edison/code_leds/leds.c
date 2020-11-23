#include "leds.h"

#include <pwm.h>
#include <stdio.h>

mraa_pwm_context pwm_green;
mraa_pwm_context pwm_red;
mraa_pwm_context pwm_blue;

void set_led_red()
{
	mraa_pwm_write(pwm_green, 0);
	mraa_pwm_write(pwm_red, 1);
	mraa_pwm_write(pwm_blue, 0);
}

void set_led_green()
{
	mraa_pwm_write(pwm_green, 1);
	mraa_pwm_write(pwm_red, 0);
	mraa_pwm_write(pwm_blue, 0);
}

void set_led_yellow()
{
	mraa_pwm_write(pwm_green, 1);
	mraa_pwm_write(pwm_red, 1);
	mraa_pwm_write(pwm_blue, 0);
}

void set_leds_off()
{
	mraa_pwm_write(pwm_green, 0);
	mraa_pwm_write(pwm_red, 0);
	mraa_pwm_write(pwm_blue, 0);
}

int init_leds()
{

	printf("Starting configuration of the LEDs\n");

	pwm_red = mraa_pwm_init(0);
	pwm_green = mraa_pwm_init(21);
	pwm_blue = mraa_pwm_init(20);

	if (pwm_green == NULL)
	{
		printf("PWM0 NULL\n");
		return -1;
	}

	if (pwm_red == NULL)
	{
		printf("PWM1 NULL\n");
		return -1;
	}

	if (pwm_blue == NULL)
	{
		printf("PWM2 NULL\n");
		return -1;
	}

	mraa_pwm_period_us(pwm_blue, 200);
	mraa_pwm_period_us(pwm_green, 200);
	mraa_pwm_period_us(pwm_red, 200);

	mraa_pwm_enable(pwm_green, 1);
	mraa_pwm_enable(pwm_red, 1);
	mraa_pwm_enable(pwm_blue, 1);

	mraa_pwm_write(pwm_green, 0);
	mraa_pwm_write(pwm_red, 0);
	mraa_pwm_write(pwm_blue, 0);

	printf("Setting the LEDs Ok\n");

	return 0;
}
