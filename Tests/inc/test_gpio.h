/**
 * @file    test_gpio.h
 * @brief   On-target test functions for the BSP GPIO driver.
 *
 * @details Each test is called from main() and validated on the
 *          NUCLEO-F410RB, either visually (LED LD2 on PA5) or by inspecting
 *          registers in the debugger SFR view.
 */

#ifndef INC_TEST_GPIO_H_
#define INC_TEST_GPIO_H_

void test_bsp_gpio_init_nominal(void);
void test_bsp_gpio_init_error_boundary_cases(void);
void test_bsp_gpio_set_pin(void);
void test_bsp_gpio_reset_pin_led_blink(void);
void test_bsp_gpio_toggle_pin_led_blink(void);
void test_bsp_gpio_read_pin(void);
void test_bsp_gpio_deinit(void);
void test_bsp_gpio_it_config_nominal(void);
void test_bsp_exti(void);
#endif /* INC_TEST_GPIO_H_ */
