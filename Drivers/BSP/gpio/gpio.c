/*
 * gpio.c
 *
 *  Created on: Mar 13, 2026
 *      Author: DANGUIAT
 */

#include "gpio.h"

GPIO_Status_t GPIO_Init(GPIO_TypeDef *port, uint8_t pin, GPIO_Config_t *config) {

    if (port == NULL)
        return BSP_GPIO_ERROR;
    if (pin > 15 || config == NULL)
        return BSP_GPIO_INVALID;

    /* Enable RCC AHB1 clock to be able to manipulate the GPIOx registers */
    if (port == GPIOA)
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    else if (port == GPIOB)
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    else if (port == GPIOC)
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    else if (port == GPIOH)
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;
    else return BSP_GPIO_INVALID;

    /* Configure MODER - applies to all modes */
    port->MODER &= ~(0x3 << (pin * 2));
    port->MODER |=  (config->mode << (pin * 2));

    /* Output or AF mode -> configure OTYPER, OSPEEDR and eventually AFR */
    if (config->mode == BSP_GPIO_MODE_OUTPUT || config->mode == BSP_GPIO_MODE_AF) {

        port->OTYPER &= ~(0x1 << pin);
        port->OTYPER |=  (config->output_type << pin);

        port->OSPEEDR &= ~(0x3 << (pin * 2));
        port->OSPEEDR |=  (config->speed << (pin * 2));

        /* AF mode : AFRL for pins 0-7, AFRH for pins 8-15 */
        if (config->mode == BSP_GPIO_MODE_AF) {
            if (pin <= 7) {
                port->AFR[0] &= ~(0xF << (pin * 4));
                port->AFR[0] |=  (config->alternate_function << (pin * 4));
            } else {
                port->AFR[1] &= ~(0xF << ((pin - 8) * 4));
                port->AFR[1] |=  (config->alternate_function << ((pin - 8) * 4));
            }
        }
    }

    /* Configure PUPDR - applies to all modes */
    port->PUPDR &= ~(0x3 << (pin * 2));
    port->PUPDR |=  (config->pull << (pin * 2));

    return BSP_GPIO_OK;
}

GPIO_Status_t GPIO_SetPin(GPIO_TypeDef *port, uint8_t pin) {
    if (port == NULL)
        return BSP_GPIO_ERROR;
    if (pin > 15)
        return BSP_GPIO_INVALID;

    port->BSRR = (0x1 << pin);

    return BSP_GPIO_OK;
}
