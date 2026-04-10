/*
 * test_gpio.h
 *
 *  Created on: Mar 13, 2026
 *      Author: DANGUIAT
 */

#ifndef INC_TEST_GPIO_H_
#define INC_TEST_GPIO_H_

void test_gpio_init_nominal(void);
void test_gpio_init_error_boundary_cases(void);
void test_gpio_set_pin(void);
void test_gpio_reset_pin_led_blink(void);
void test_gpio_toggle_pin_led_blink(void);
void test_gpio_read_pin(void);
void test_gpio_deinit(void);
#endif /* INC_TEST_GPIO_H_ */
