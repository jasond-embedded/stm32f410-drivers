/**
 * @file    gpio.c
 * @brief   Register-level GPIO driver for the STM32F410RB - implementation.
 *
 * @details Every public function validates its parameters before any
 *          register access and returns a BSP_GPIO_Status_t code.
 *          Register fields are always cleared before being set.
 *          BSP_GPIO_TogglePin() uses a non-atomic read-modify-write on ODR
 *          (documented limitation).
 *          BSP_GPIO_EXTI_Callback() is weak and must be overridden by the
 *          application.
 *
 * @see     RM0401 Rev4, sections 6.4, 7.2, 9.3
 */

#include "gpio.h"

BSP_GPIO_Status_t BSP_GPIO_Init(GPIO_TypeDef *port, uint8_t pin, BSP_GPIO_Config_t *config) {

    if (port == NULL)
        return BSP_GPIO_ERROR;
    if ((pin > 15) || (config == NULL)) {
    	return BSP_GPIO_INVALID;
    }

	/* Validate different enums*/
	if (config->mode > BSP_GPIO_MODE_ANALOG)
		return BSP_GPIO_INVALID;
	if (config->output_type > BSP_GPIO_OTYPE_OPEN_DRAIN)
		return BSP_GPIO_INVALID;
	if (config->speed > BSP_GPIO_SPEED_VERY_HIGH)
		return BSP_GPIO_INVALID;
	if (config->pull > BSP_GPIO_PUPD_PULL_DOWN)
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
    port->MODER &= ~((uint32_t)0x3U << (pin * 2U));
    port->MODER |=  ((uint32_t)config->mode << (pin * 2U));

    /* Output or AF mode -> configure OTYPER, OSPEEDR and eventually AFR */
    if (config->mode == BSP_GPIO_MODE_OUTPUT || config->mode == BSP_GPIO_MODE_AF) {

        port->OTYPER &= ~((uint32_t)0x1U << pin);
        port->OTYPER |=  ((uint32_t)config->output_type << pin);

        port->OSPEEDR &= ~((uint32_t)0x3U << (pin * 2U));
        port->OSPEEDR |=  ((uint32_t)config->speed << (pin * 2U));

        /* AF mode : AFRL for pins 0-7, AFRH for pins 8-15 */
        if (config->mode == BSP_GPIO_MODE_AF) {
            if (pin <= 7) {
                port->AFR[0] &= ~((uint32_t)0xFU << (pin * 4U));
                port->AFR[0] |=  ((uint32_t)config->alternate_function << (pin * 4U));
            } else {
                port->AFR[1] &= ~((uint32_t)0xFU << ((pin - 8U) * 4U));
                port->AFR[1] |=  ((uint32_t)config->alternate_function << ((pin - 8U) * 4U));
            }
        }
    }

    /* Configure PUPDR - applies to all modes */
    port->PUPDR &= ~((uint32_t)0x3U << (pin * 2U));
    port->PUPDR |=  ((uint32_t)config->pull << (pin * 2U));

    return BSP_GPIO_OK;
}

BSP_GPIO_Status_t BSP_GPIO_DeInit(GPIO_TypeDef *port, uint8_t pin) {
    if (port == NULL)
        return BSP_GPIO_ERROR;
    if (pin > 15U)
        return BSP_GPIO_INVALID;

    /* Reset the IO pin to Input floating mode (as default) */
    port->MODER &= ~((uint32_t)0x3U << (pin * 2U));

    /* Reset the IO pin to Push-Pull output type (as default) */
    port->OTYPER &= ~((uint32_t)0x1U << pin);

    /* Reset the IO pin to Low Speed (as default) */
    port->OSPEEDR &= ~((uint32_t)0x3U << (pin * 2U));

    /* Deactivate the Pull-up and the Pull-down resistor (as default) */
    port->PUPDR &= ~((uint32_t)0x3U << (pin * 2U));

    /* Reset the Alternate function to AF0 (as default) */
    if (pin <= 7) {
    	port->AFR[0] &= ~((uint32_t)0xFU << (pin * 4U));
    } else {
    	port->AFR[1] &= ~((uint32_t)0xFU << ((pin - 8U) * 4U));
    }

    /* Note: RCC clock is not disabled here as other pins on this port
     * may still be in use. Disable manually via RCC->AHB1ENR if needed. */
    return BSP_GPIO_OK;
}

BSP_GPIO_Status_t BSP_GPIO_SetPin(GPIO_TypeDef *port, uint8_t pin) {
    if (port == NULL)
        return BSP_GPIO_ERROR;
    if (pin > 15)
        return BSP_GPIO_INVALID;

    port->BSRR = ((uint32_t)0x1U << pin);

    return BSP_GPIO_OK;
}

BSP_GPIO_Status_t BSP_GPIO_ResetPin(GPIO_TypeDef *port, uint8_t pin) {
    if (port == NULL)
        return BSP_GPIO_ERROR;
    if (pin > 15)
        return BSP_GPIO_INVALID;

    port->BSRR = ((uint32_t)0x1U << (pin + 16U));

    return BSP_GPIO_OK;
}

/* Note : This is not an atomic Toggle, an interrupt can occur during the Read-Modify-Write instructions */
BSP_GPIO_Status_t BSP_GPIO_TogglePin(GPIO_TypeDef *port, uint8_t pin) {
    if (port == NULL)
        return BSP_GPIO_ERROR;
    if (pin > 15)
        return BSP_GPIO_INVALID;
    port->ODR ^= ((uint32_t)0x1U << pin);						/* Non atomic*/

    return BSP_GPIO_OK;
}


BSP_GPIO_Status_t BSP_GPIO_ReadPin(GPIO_TypeDef *port, uint8_t pin, uint8_t *value) {
    if (port == NULL)
        return BSP_GPIO_ERROR;
    if (pin > 15)
        return BSP_GPIO_INVALID;
    if (value == NULL)
    	return BSP_GPIO_INVALID;

    *value = (port->IDR >> pin) & 0x1U;

    return BSP_GPIO_OK;
}

BSP_GPIO_Status_t BSP_GPIO_LockPin(GPIO_TypeDef *port, uint8_t pin) {
    if (port == NULL)
        return BSP_GPIO_ERROR;
    if (pin > 15)
        return BSP_GPIO_INVALID;

    __IO uint32_t tmp = ((uint32_t)0x1U << 16U) | ((uint32_t)0x1U << pin); // declare it as __IO (volatile) to force the processor not to optimize it and execute each write and read, even if not used.

    /* WR LCKR[16] = ‘1’ + LCKR[15:0] */
    port->LCKR = tmp;

    /* WR LCKR[16] = ‘0’ + LCKR[15:0] */
    port->LCKR= ((uint32_t)0x1U << pin);

    /* WR LCKR[16] = ‘1’ + LCKR[15:0] */
    port->LCKR= tmp;

    tmp = port->LCKR;

    if ((port->LCKR & ((uint32_t)0x1U << 16U)) != 0) {
    	return BSP_GPIO_OK;
    }
    else return BSP_GPIO_ERROR;
}


BSP_GPIO_Status_t BSP_GPIO_EXTI_Config(GPIO_TypeDef *port, uint8_t pin, BSP_GPIO_IT_Trigger_t trigger, uint32_t priority) {
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

    /* EXTI clock enable */
    RCC->APB2ENR |= RCC_APB2ENR_EXTITEN;

    uint8_t cr_port_val;

    /* SYSCFG_EXTICRx — mapper port → ligne EXTI */
    if (port == GPIOA) cr_port_val = 0U;
    else if (port == GPIOB) cr_port_val = 1U;
    else if (port == GPIOH) cr_port_val = 7U;
    else if (port == GPIOC) cr_port_val = 2U;
    else return BSP_GPIO_INVALID;

   uint8_t exti_idx = pin >> 2U; // integer division by 4
   uint8_t exti_pos = (pin % 4U) << 2U; // Do not forget that EXTICR[0] includes EXTI0, EXTI1, EXTI2, EXTI3 (4-bits words)
   SYSCFG->EXTICR[exti_idx] &= ~((uint32_t)0xFU << exti_pos);
   SYSCFG->EXTICR[exti_idx] |= ((uint32_t)cr_port_val << exti_pos);

    /* EXTI_RTSR / FTSR — Configure trigger*/
   EXTI->RTSR &= ~((uint32_t)0x1U << pin);
   EXTI->FTSR &= ~((uint32_t)0x1U << pin);

   if (trigger == BSP_GPIO_IT_RISING || trigger == BSP_GPIO_IT_BOTH) {
	   EXTI->RTSR |= ((uint32_t)0x1U << pin);
   }
   if (trigger == BSP_GPIO_IT_FALLING || trigger == BSP_GPIO_IT_BOTH) {
	   EXTI->FTSR |= ((uint32_t)0x1U << pin);
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
   uint32_t read_back = NVIC_GetPriority(exti_irqn);
   UNUSED(read_back);
   __NOP();

   return BSP_GPIO_OK;
}


BSP_GPIO_Status_t BSP_GPIO_EXTI_Enable(uint8_t pin) {
	if (pin > 15)
		return BSP_GPIO_INVALID;

    /* EXTI_IMR — activate the line */
   EXTI->IMR |= ((uint32_t)0x1U << pin);

   return BSP_GPIO_OK;
}

BSP_GPIO_Status_t BSP_GPIO_EXTI_Disable(uint8_t pin) {
	if (pin > 15)
		return BSP_GPIO_INVALID;

    /* EXTI_IMR — activate the line */
   EXTI->IMR &= ~((uint32_t)0x1U << pin);

   return BSP_GPIO_OK;
}

BSP_GPIO_Status_t BSP_GPIO_EXTI_IRQHandler(uint8_t pin) {

	/* EXTI line interrupt detected */
	if ((EXTI->PR & ((uint32_t)0x1U << pin)) != 0) {
		EXTI->PR = ((uint32_t)0x1U << pin); // Clear IT
		BSP_GPIO_EXTI_Callback(pin);
	}
	return BSP_GPIO_OK;
}

/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin Specifies the pins connected EXTI line
  * @retval None
  */
__weak void BSP_GPIO_EXTI_Callback(uint8_t pin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(pin);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the BSP_GPIO_EXTI_Callback could be implemented in the user file
   */
}







