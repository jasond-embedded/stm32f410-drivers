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

	/* Validate different enums*/
	if (config->mode > BSP_GPIO_MODE_ANALOG)
		return BSP_GPIO_INVALID;
	if (config->output_type > BSP_GPIO_OTYPE_OPEN_DRAIN)
		return BSP_GPIO_INVALID;
	if (config->speed > BSP_GPIO_SPEED_VERY_HIGH)
		return BSP_GPIO_INVALID;
	if (config->pull > BSP_GPIO_PUPD_RESERVED)
		return BSP_GPIO_INVALID;
	if (config->alternate_function > BSP_GPIO_AF15)
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

GPIO_Status_t GPIO_DeInit(GPIO_TypeDef *port, uint8_t pin) {
    if (port == NULL)
        return BSP_GPIO_ERROR;
    if (pin > 15U)
        return BSP_GPIO_INVALID;

    /* Reset the IO pin to Input floating mode (as default) */
    port->MODER &= ~(0x3U << (pin * 2U));

    /* Reset the IO pin to Push-Pull output type (as default) */
    port->OTYPER &= ~(0x1U << pin);

    /* Reset the IO pin to Low Speed (as default) */
    port->OSPEEDR &= ~(0x3U << (pin * 2U));

    /* Deactivate the Pull-up and the Pull-down resistor (as default) */
    port->PUPDR &= ~(0x3U << (pin * 2U));

    /* Reset the Alternate function to AF0 (as default) */
    if (pin <= 7) {
    	port->AFR[0] &= ~(0xF << (pin * 4));
    } else {
    	port->AFR[1] &= ~(0xF << ((pin - 8) * 4));
    }

    /* Note: RCC clock is not disabled here as other pins on this port
     * may still be in use. Disable manually via RCC->AHB1ENR if needed. */
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

GPIO_Status_t GPIO_ResetPin(GPIO_TypeDef *port, uint8_t pin) {
    if (port == NULL)
        return BSP_GPIO_ERROR;
    if (pin > 15)
        return BSP_GPIO_INVALID;

    port->BSRR = (0x1 << (pin + 16));

    return BSP_GPIO_OK;
}

/* Note : This is not an atomic Toggle, an interrupt can occur during the Read-Modify-Write instructions */
GPIO_Status_t GPIO_TogglePin(GPIO_TypeDef *port, uint8_t pin) {
    if (port == NULL)
        return BSP_GPIO_ERROR;
    if (pin > 15)
        return BSP_GPIO_INVALID;
    port->ODR ^= (0x1 << pin);						/* Non atomic*/

    return BSP_GPIO_OK;
}


GPIO_Status_t GPIO_ReadPin(GPIO_TypeDef *port, uint8_t pin, uint8_t *value) {
    if (port == NULL)
        return BSP_GPIO_ERROR;
    if (pin > 15)
        return BSP_GPIO_INVALID;
    if (value == NULL)
    	return BSP_GPIO_INVALID;

    *value = (port->IDR >> pin) & 0x1;

    return BSP_GPIO_OK;
}

GPIO_Status_t GPIO_LockPin(GPIO_TypeDef *port, uint8_t pin) {
    if (port == NULL)
        return BSP_GPIO_ERROR;
    if (pin > 15)
        return BSP_GPIO_INVALID;

    __IO uint32_t tmp = (0x1UL << 16U) | (0x1UL << pin); // declare it as __IO (volatile) to force the processor not to optimize it and execute each write and read, even if not used.

    /* WR LCKR[16] = ‘1’ + LCKR[15:0] */
    port->LCKR = tmp;

    /* WR LCKR[16] = ‘0’ + LCKR[15:0] */
    port->LCKR= (0x1UL << pin);

    /* WR LCKR[16] = ‘1’ + LCKR[15:0] */
    port->LCKR= tmp;

    tmp = port->LCKR;

    if (port->LCKR & (0x1UL << 16U) == 0x1UL) {
    	return BSP_GPIO_OK;
    }
    else return BSP_GPIO_ERROR;
}


GPIO_Status_t GPIO_IT_Config(GPIO_TypeDef *port, uint8_t pin, GPIO_IT_Trigger_t trigger, uint32_t priority) {
    if (port == NULL)
        return BSP_GPIO_ERROR;
    if (pin > 15)
        return BSP_GPIO_INVALID;
    if (priority > 15)
    	return BSP_GPIO_INVALID;
    if(trigger > BSP_GPIO_IT_BOTH)
    	return BSP_GPIO_INVALID;

    /* SYSCFG clock enable */
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    /* SYSCFG EXTI clock enable */
    RCC->APB2ENR |= RCC_APB2ENR_EXTITEN;

    uint8_t cr_port_val;

    /* SYSCFG_EXTICRx — mapper port → ligne EXTI */
    if (port == GPIOA) cr_port_val = 0U;
    else if (port == GPIOB) cr_port_val = 1U;
    else if (port == GPIOH) cr_port_val = 7U;
    else if (port == GPIOC) cr_port_val = 2U;
    else return BSP_GPIO_INVALID;

   uint8_t exti_idx = pin >> 2U; // integer division by 4
   uint8_t exti_pos = (pin % 4) << 2U; // Do not forget that EXTICR[0] includes EXTI0, EXTI1, EXTI2, EXTI3 (4-bits words)
   SYSCFG->EXTICR[exti_idx] &= ~(0xFU << exti_pos);
   SYSCFG->EXTICR[exti_idx] |= (cr_port_val << exti_pos);

    /* EXTI_IMR — activate the line */
   EXTI->IMR |= (1U << pin);

    /* EXTI_RTSR / FTSR — Configure trigger*/
   EXTI->RTSR &= ~(1U << pin);
   EXTI->FTSR &= ~(1U << pin);

   if (trigger == BSP_GPIO_IT_RISING || trigger == BSP_GPIO_IT_BOTH) {
	   EXTI->RTSR |= (1U << pin);
   }
   if (trigger == BSP_GPIO_IT_FALLING || trigger == BSP_GPIO_IT_BOTH) {
	   EXTI->FTSR |= (1U << pin);
   }
   /* If configured as BSP_GPIO_IT_NONE, do nothing since both EXTI_RTSR and EXTI_FTSR were cleared */

    /* NVIC_SetPriority + NVIC_EnableIRQ */
   IRQn_Type exti_irqn;
   if      (pin == 0)                      exti_irqn = EXTI0_IRQn;
   else if (pin == 1)                      exti_irqn = EXTI1_IRQn;
   else if (pin == 2)                      exti_irqn = EXTI2_IRQn;
   else if (pin == 3)                      exti_irqn = EXTI3_IRQn;
   else if (pin == 4)                      exti_irqn = EXTI4_IRQn;
   else if (pin >= 5  && pin <= 9)         exti_irqn = EXTI9_5_IRQn;
   else                                    exti_irqn = EXTI15_10_IRQn;

   NVIC_SetPriority(exti_irqn, priority);
   NVIC_EnableIRQ(exti_irqn);

   return BSP_GPIO_OK;
}






