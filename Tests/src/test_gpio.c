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


