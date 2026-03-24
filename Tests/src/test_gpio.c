/*
 * test_gpio.c
 *
 *  Created on: Mar 13, 2026
 *      Author: DANGUIAT
 */

#include "test_gpio.h"
#include "gpio.h"

void test_gpio_config_led_blink(void) {
    GPIO_Config_t config = {
        .mode        = BSP_GPIO_MODE_OUTPUT,
        .output_type = BSP_GPIO_OTYPE_PUSH_PULL,
        .speed       = BSP_GPIO_SPEED_LOW,
        .pull        = BSP_GPIO_PUPD_NONE,
        .alternate_function = BSP_GPIO_AF0
    };

    GPIO_Init(GPIOA, 5, &config);

    while(1) {
        GPIOA->BSRR = (1 << 5);
        HAL_Delay(500);
        GPIOA->BSRR = (1 << 21);
        HAL_Delay(500);
    }
}

/* GPIO_Init() function must be validated before testing GPIO_SetPin() */
void test_gpio_set_pin(void) {
    GPIO_Config_t config = {
        .mode        = BSP_GPIO_MODE_OUTPUT,
        .output_type = BSP_GPIO_OTYPE_PUSH_PULL,
        .speed       = BSP_GPIO_SPEED_LOW,
        .pull        = BSP_GPIO_PUPD_NONE,
        .alternate_function = BSP_GPIO_AF0
    };

    GPIO_Init(GPIOA, 5, &config);
    GPIO_SetPin(GPIOA, 5);
}

/* GPIO_Init() and GPIO_SetPin() function must be validated before testing GPIO_ResetPin*/
void test_gpio_reset_pin_led_blink(void) {
    GPIO_Config_t config = {
        .mode        = BSP_GPIO_MODE_OUTPUT,
        .output_type = BSP_GPIO_OTYPE_PUSH_PULL,
        .speed       = BSP_GPIO_SPEED_LOW,
        .pull        = BSP_GPIO_PUPD_NONE,
        .alternate_function = BSP_GPIO_AF0
    };

    GPIO_Init(GPIOA, 5, &config);
    while(1) {
        GPIO_SetPin(GPIOA, 5);
        HAL_Delay(500);
        GPIO_ResetPin(GPIOA, 5);
        HAL_Delay(500);
    }
}

/* GPIO_Init() function must be validated before testing GPIO_TogglePin() */
void test_gpio_toogle_pin_led_blink(void) {
    GPIO_Config_t config = {
        .mode        = BSP_GPIO_MODE_OUTPUT,
        .output_type = BSP_GPIO_OTYPE_PUSH_PULL,
        .speed       = BSP_GPIO_SPEED_LOW,
        .pull        = BSP_GPIO_PUPD_NONE,
        .alternate_function = BSP_GPIO_AF0
    };

	GPIO_Init(GPIOA, 5, &config);

    while(1) {
    	GPIO_TogglePin(GPIOA, 5);
    	HAL_Delay(500);
    	GPIO_TogglePin(GPIOA, 5);
    	HAL_Delay(500);
    }
}

/* GPIO_Init() and GPIO_SetPin() functions must be validated before testing GPIO_TogglePin().
 * How does the test work ? The value of PA8 is read, if it is 1 (connected to 3v3), then the LED is on.
 * If the value read on PA8 is 0 (not connected to 3v3, but to the pull-down by default (see the config_input), then the LED is off.
 * We can also just watch the value of input_state in debug mode to see if it is well read.
 */
void test_gpio_read_pin(void) {
    GPIO_Config_t config_input = {
        .mode        = BSP_GPIO_MODE_INPUT,
        .pull        = BSP_GPIO_PUPD_PULL_DOWN,
    };

    GPIO_Config_t config_output = {
        .mode        = BSP_GPIO_MODE_OUTPUT,
        .output_type = BSP_GPIO_OTYPE_PUSH_PULL,
        .speed       = BSP_GPIO_SPEED_LOW,
        .pull        = BSP_GPIO_PUPD_NONE,
        .alternate_function = BSP_GPIO_AF0
    };

	GPIO_Init(GPIOA, 5, &config_output);
	GPIO_Init(GPIOA, 8, &config_input);
	uint8_t input_state = 0;
	while(1) {
		GPIO_ReadPin(GPIOA, 8, &input_state);
		if (input_state == 1) {
			GPIO_SetPin(GPIOA, 5);
		}
		else {
			GPIO_ResetPin(GPIOA, 5);
		}
	}

}


