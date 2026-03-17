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

    while(1) {
        GPIO_Init(GPIOA, 5, &config);
        GPIO_SetPin(GPIOA, 5);
        HAL_Delay(500);
        GPIO_ResetPin(GPIOA, 5);
        HAL_Delay(500);
    }

}


