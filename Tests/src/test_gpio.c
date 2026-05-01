/*
 * test_gpio.c
 *
 *  Created on: Mar 13, 2026
 *      Author: DANGUIAT
 */

#include "test_gpio.h"
#include "gpio.h"

/**
 * @brief  Validate BSP_GPIO_Init() for nominal cases
 *
 * @note   Debug validation:
 *           Set breakpoints on each BSP_GPIO_Init() call
 *           Watch variables : status = BSP_GPIO_OK
 *           Watch register values : each port register in SFRs view
 * 			 Expected register values after BSP_GPIO_Init() call (for GPIOA 5 pin and GPIOA 8)
 *
 * 			 config0  -> MODER  bits [11:10] = 0b01  (OUTPUT)
 *             			 OTYPER bit  [5]     = 0b0   (PUSH_PULL)
 *             			 OSPEEDR bits[11:10] = 0b00  (LOW)
 *             			 PUPDR  bits [11:10] = 0b00  (NONE)
 *
 * 			 config1  -> MODER  bits [11:10] = 0b00  (INPUT)
 *            			  PUPDR  bits [11:10] = 0b00  (NONE)
 *
 * 			 config2  -> MODER  bits [11:10] = 0b11  (ANALOG)
 *
 * 			 config3  -> MODER  bits [11:10] = 0b10  (AF)
 *             			 OTYPER bit  [5]     = 0b0   (PUSH_PULL)
 *            			 OSPEEDR bits[11:10] = 0b00  (LOW)
 *            			 AFR[0] bits [23:20] = 0x7   (AF7) for pin 5
 *
 * 			 config4  -> OTYPER bit  [5]     = 0b1   (OPEN_DRAIN)
 *
 * 			 config5  -> OSPEEDR bits[11:10] = 0b01  (MEDIUM)
 *
 * 			 config6  -> OSPEEDR bits[11:10] = 0b10  (HIGH)
 *
 * 			 config7  -> OSPEEDR bits[11:10] = 0b11  (VERY_HIGH)
 *
 * 			 config8  -> PUPDR  bits [11:10] = 0b01  (PULL_UP)
 *
 * 			 config9  -> PUPDR  bits [11:10] = 0b10  (PULL_DOWN)
 *
 * 			 config10 -> MODER  bits [17:16] = 0b10  (AF)        for pin 8
 *             			 AFR[1] bits [3:0]   = 0x7   (AF7)       for pin 8

 *
 * @note   No hardware setup required — all cases return before
 *           accessing any register
 *
 */
void test_bsp_gpio_init_nominal(void) {
	BSP_GPIO_Status_t status;
	/* --- MODER --- */
	/* config0 : OUTPUT - all MODER values covered by configs below */
	BSP_GPIO_Config_t config0 = {
	    .mode        = BSP_GPIO_MODE_OUTPUT,
	    .output_type = BSP_GPIO_OTYPE_PUSH_PULL,
	    .speed       = BSP_GPIO_SPEED_LOW,
	    .pull        = BSP_GPIO_PUPD_NONE,
	    .alternate_function = BSP_GPIO_AF0
	};

	BSP_GPIO_Config_t config1 = {
	    .mode        = BSP_GPIO_MODE_INPUT,
	    .output_type = BSP_GPIO_OTYPE_PUSH_PULL,  /* ignored in INPUT mode */
	    .speed       = BSP_GPIO_SPEED_LOW,         /* ignored in INPUT mode */
	    .pull        = BSP_GPIO_PUPD_NONE,
	    .alternate_function = BSP_GPIO_AF0
	};

	BSP_GPIO_Config_t config2 = {
	    .mode        = BSP_GPIO_MODE_ANALOG,
	    .output_type = BSP_GPIO_OTYPE_PUSH_PULL,  /* ignored in ANALOG mode */
	    .speed       = BSP_GPIO_SPEED_LOW,         /* ignored in ANALOG mode */
	    .pull        = BSP_GPIO_PUPD_NONE,
	    .alternate_function = BSP_GPIO_AF0
	};

	BSP_GPIO_Config_t config3 = {
	    .mode        = BSP_GPIO_MODE_AF,
	    .output_type = BSP_GPIO_OTYPE_PUSH_PULL,
	    .speed       = BSP_GPIO_SPEED_LOW,
	    .pull        = BSP_GPIO_PUPD_NONE,
	    .alternate_function = BSP_GPIO_AF7        /* e.g. USART */
	};

	/* --- OTYPER (OUTPUT and AF modes only) --- */
	/* config0 already covers PUSH_PULL */
	BSP_GPIO_Config_t config4 = {
	    .mode        = BSP_GPIO_MODE_OUTPUT,
	    .output_type = BSP_GPIO_OTYPE_OPEN_DRAIN,
	    .speed       = BSP_GPIO_SPEED_LOW,
	    .pull        = BSP_GPIO_PUPD_NONE,
	    .alternate_function = BSP_GPIO_AF0
	};

	/* --- OSPEEDR (OUTPUT and AF modes only) --- */
	/* config0 already covers SPEED_LOW */
	BSP_GPIO_Config_t config5 = {
	    .mode        = BSP_GPIO_MODE_OUTPUT,
	    .output_type = BSP_GPIO_OTYPE_PUSH_PULL,
	    .speed       = BSP_GPIO_SPEED_MEDIUM,
	    .pull        = BSP_GPIO_PUPD_NONE,
	    .alternate_function = BSP_GPIO_AF0
	};

	BSP_GPIO_Config_t config6 = {
	    .mode        = BSP_GPIO_MODE_OUTPUT,
	    .output_type = BSP_GPIO_OTYPE_PUSH_PULL,
	    .speed       = BSP_GPIO_SPEED_HIGH,
	    .pull        = BSP_GPIO_PUPD_NONE,
	    .alternate_function = BSP_GPIO_AF0
	};

	BSP_GPIO_Config_t config7 = {
	    .mode        = BSP_GPIO_MODE_OUTPUT,
	    .output_type = BSP_GPIO_OTYPE_PUSH_PULL,
	    .speed       = BSP_GPIO_SPEED_VERY_HIGH,
	    .pull        = BSP_GPIO_PUPD_NONE,
	    .alternate_function = BSP_GPIO_AF0
	};

	/* --- PUPDR --- */
	/* config0 already covers PUPD_NONE */
	BSP_GPIO_Config_t config8 = {
	    .mode        = BSP_GPIO_MODE_INPUT,
	    .output_type = BSP_GPIO_OTYPE_PUSH_PULL,  /* ignored in INPUT mode */
	    .speed       = BSP_GPIO_SPEED_LOW,         /* ignored in INPUT mode */
	    .pull        = BSP_GPIO_PUPD_PULL_UP,
	    .alternate_function = BSP_GPIO_AF0
	};

	BSP_GPIO_Config_t config9 = {
	    .mode        = BSP_GPIO_MODE_INPUT,
	    .output_type = BSP_GPIO_OTYPE_PUSH_PULL,  /* ignored in INPUT mode */
	    .speed       = BSP_GPIO_SPEED_LOW,         /* ignored in INPUT mode */
	    .pull        = BSP_GPIO_PUPD_PULL_DOWN,
	    .alternate_function = BSP_GPIO_AF0
	};

	/* --- AFR (AF mode only) --- */
	/* config3 covers AF on pin <= 7 (AFRL) */
	/* config10 covers AF on pin >= 8 (AFRH) - same config, different pin */
	BSP_GPIO_Config_t config10 = {
	    .mode        = BSP_GPIO_MODE_AF,
	    .output_type = BSP_GPIO_OTYPE_PUSH_PULL,
	    .speed       = BSP_GPIO_SPEED_HIGH,
	    .pull        = BSP_GPIO_PUPD_NONE,
	    .alternate_function = BSP_GPIO_AF7        /* e.g. USART on pin >= 8 */
	};

	status = BSP_GPIO_Init(GPIOA, 5, &config1);
	status = BSP_GPIO_Init(GPIOA, 5, &config0);
	status = BSP_GPIO_Init(GPIOA, 5, &config2);
	status = BSP_GPIO_Init(GPIOA, 5, &config3);
	status = BSP_GPIO_Init(GPIOA, 5, &config4);
	status = BSP_GPIO_Init(GPIOA, 5, &config5);
	status = BSP_GPIO_Init(GPIOA, 5, &config6);
	status = BSP_GPIO_Init(GPIOA, 5, &config7);
	status = BSP_GPIO_Init(GPIOA, 5, &config8);
	status = BSP_GPIO_Init(GPIOA, 5, &config9);
	status = BSP_GPIO_Init(GPIOA, 8, &config10);
}

/**
 * @brief  Validate BSP_GPIO_Init() error handling and boundary cases
 *
 * @note   Debug validation:
 *           Set breakpoints on each BSP_GPIO_Init() call
 *           Watch variable : status
 *           All calls must return BSP_GPIO_ERROR or BSP_GPIO_INVALID
 *           None must return BSP_GPIO_OK
 *
 * @note   No hardware setup required — all cases return before
 *           accessing any register
 *
 */
void test_bsp_gpio_init_error_boundary_cases(void) {
    BSP_GPIO_Status_t status;

    BSP_GPIO_Config_t valid_config = {
        .mode               = BSP_GPIO_MODE_OUTPUT,
        .output_type        = BSP_GPIO_OTYPE_PUSH_PULL,
        .speed              = BSP_GPIO_SPEED_LOW,
        .pull               = BSP_GPIO_PUPD_NONE,
        .alternate_function = BSP_GPIO_AF0
    };

    /* --- BSP_GPIO_ERROR cases --- */

    /* port == NULL -> BSP_GPIO_ERROR */
    status = BSP_GPIO_Init(NULL, 5, &valid_config);
    /* Expected : BSP_GPIO_ERROR */

    /* --- BSP_GPIO_INVALID cases --- */

    /* pin > 15 -> BSP_GPIO_INVALID */
    status = BSP_GPIO_Init(GPIOA, 16, &valid_config);
    /* Expected : BSP_GPIO_INVALID */

    /* config == NULL -> BSP_GPIO_INVALID */
    status = BSP_GPIO_Init(GPIOA, 5, NULL);
    /* Expected : BSP_GPIO_INVALID */

    /* config.mode > 3, out of range -> BSP_GPIO_INVALID */
    BSP_GPIO_Config_t invalid_mode = {
        .mode               = 0x04,
        .output_type        = BSP_GPIO_OTYPE_PUSH_PULL,
        .speed              = BSP_GPIO_SPEED_LOW,
        .pull               = BSP_GPIO_PUPD_NONE,
        .alternate_function = BSP_GPIO_AF0
    };
    status = BSP_GPIO_Init(GPIOA, 5, &invalid_mode);
    /* Expected : BSP_GPIO_INVALID */

    /* config.output_type > 1, out of range -> BSP_GPIO_INVALID */
    BSP_GPIO_Config_t invalid_otype = {
        .mode               = BSP_GPIO_MODE_OUTPUT,
        .output_type        = 0x02,
        .speed              = BSP_GPIO_SPEED_LOW,
        .pull               = BSP_GPIO_PUPD_NONE,
        .alternate_function = BSP_GPIO_AF0
    };
    status = BSP_GPIO_Init(GPIOA, 5, &invalid_otype);
    /* Expected : BSP_GPIO_INVALID */

    /* config.speed >  out of range -> BSP_GPIO_INVALID */
    BSP_GPIO_Config_t invalid_speed = {
        .mode               = BSP_GPIO_MODE_OUTPUT,
        .output_type        = BSP_GPIO_OTYPE_PUSH_PULL,
        .speed              = 0x04,
        .pull               = BSP_GPIO_PUPD_NONE,
        .alternate_function = BSP_GPIO_AF0
    };
    status = BSP_GPIO_Init(GPIOA, 5, &invalid_speed);
    /* Expected : BSP_GPIO_INVALID */

    /* config.pull out of range -> BSP_GPIO_INVALID */
    BSP_GPIO_Config_t invalid_pull = {
        .mode               = BSP_GPIO_MODE_OUTPUT,
        .output_type        = BSP_GPIO_OTYPE_PUSH_PULL,
        .speed              = BSP_GPIO_SPEED_LOW,
        .pull               = 0x04,
        .alternate_function = BSP_GPIO_AF0
    };
    status = BSP_GPIO_Init(GPIOA, 5, &invalid_pull);
    /* Expected : BSP_GPIO_INVALID */

    /* config.alternate_function out of range -> BSP_GPIO_INVALID */
    BSP_GPIO_Config_t invalid_af = {
        .mode               = BSP_GPIO_MODE_OUTPUT,
        .output_type        = BSP_GPIO_OTYPE_PUSH_PULL,
        .speed              = BSP_GPIO_SPEED_LOW,
        .pull               = BSP_GPIO_PUPD_NONE,
        .alternate_function = 0x10
    };
    status = BSP_GPIO_Init(GPIOA, 5, &invalid_af);
    /* Expected : BSP_GPIO_INVALID */
}

/**
 * @brief  Validate BSP_GPIO_SetPin() by setting PA5 (LED) to logical high
 *
 * @note   Hardware setup:
 *           - PA5 : LED output (push-pull, no pull)
 *             Expected : LED turns on and stays on
 *
 * @note   Debug validation:
 *           Set breakpoint after BSP_GPIO_SetPin() call
 *           Register check : GPIOA->ODR bit [5] = 1
 *
 * @note   Dependencies: BSP_GPIO_Init() must be validated first
 */
void test_bsp_gpio_set_pin(void) {
    BSP_GPIO_Config_t config = {
        .mode        = BSP_GPIO_MODE_OUTPUT,
        .output_type = BSP_GPIO_OTYPE_PUSH_PULL,
        .speed       = BSP_GPIO_SPEED_LOW,
        .pull        = BSP_GPIO_PUPD_NONE,
        .alternate_function = BSP_GPIO_AF0
    };

    BSP_GPIO_Init(GPIOA, 5, &config);
    BSP_GPIO_SetPin(GPIOA, 5);
}

/**
 * @brief  Validate BSP_GPIO_ResetPin() by blinking PA5 (LED) using
 *         BSP_GPIO_SetPin() and BSP_GPIO_ResetPin() alternately
 *
 * @note   Hardware setup:
 *           - PA5 : LED output (push-pull, no pull)
 *             Expected : LED blinks at 1Hz (500ms on, 500ms off)
 *
 * @note   Debug validation:
 *           Set breakpoint on BSP_GPIO_SetPin() and BSP_GPIO_ResetPin() calls
 *           Register check : GPIOA->ODR bit [5] toggles between 1 and 0
 *
 * @note   Dependencies: BSP_GPIO_Init() and BSP_GPIO_SetPin() must be validated first
 */
void test_bsp_gpio_reset_pin_led_blink(void) {
    BSP_GPIO_Config_t config = {
        .mode        = BSP_GPIO_MODE_OUTPUT,
        .output_type = BSP_GPIO_OTYPE_PUSH_PULL,
        .speed       = BSP_GPIO_SPEED_LOW,
        .pull        = BSP_GPIO_PUPD_NONE,
        .alternate_function = BSP_GPIO_AF0
    };

    BSP_GPIO_Init(GPIOA, 5, &config);
    while(1) {
        BSP_GPIO_SetPin(GPIOA, 5);
        HAL_Delay(500);
        BSP_GPIO_ResetPin(GPIOA, 5);
        HAL_Delay(500);
    }
}

/* BSP_GPIO_Init() function must be validated before testing BSP_GPIO_TogglePin() */
void test_bsp_gpio_toggle_pin_led_blink(void) {
    BSP_GPIO_Config_t config = {
        .mode        = BSP_GPIO_MODE_OUTPUT,
        .output_type = BSP_GPIO_OTYPE_PUSH_PULL,
        .speed       = BSP_GPIO_SPEED_LOW,
        .pull        = BSP_GPIO_PUPD_NONE,
        .alternate_function = BSP_GPIO_AF0
    };

	BSP_GPIO_Init(GPIOA, 5, &config);

    while(1) {
    	BSP_GPIO_TogglePin(GPIOA, 5);
    	HAL_Delay(500);
    }
}

/**
 * @brief  Validate BSP_GPIO_ReadPin() by reading PA8 state and
 *         reflecting it on PA5 (LED)
 *
 * @note   Hardware setup:
 *           - PA5 : LED output (push-pull, no pull)
 *           - PA8 : input (pull-down enabled)
 *             Connect PA8 to 3V3 to simulate logical 1 -> LED on
 *             Connect PA8 to GND or leave floating -> LED off
 *             DO NOT leave PA8 floating without pull-down — undefined state
 *
 * @note   Debug validation:
 *           Set breakpoint on BSP_GPIO_ReadPin() call
 *           Watch variable : input_state
 *           Expected       : 1 when PA8 connected to 3V3
 *                            0 when PA8 connected to GND
 *           Register check : GPIOA->IDR bit [8] must match input_state
 *
 * @note   Dependencies: BSP_GPIO_Init() and BSP_GPIO_SetPin() must be validated first
 */
void test_bsp_gpio_read_pin(void) {
    BSP_GPIO_Config_t config_input = {
        .mode        = BSP_GPIO_MODE_INPUT,
        .pull        = BSP_GPIO_PUPD_PULL_DOWN,
    };

    BSP_GPIO_Config_t config_output = {
        .mode        = BSP_GPIO_MODE_OUTPUT,
        .output_type = BSP_GPIO_OTYPE_PUSH_PULL,
        .speed       = BSP_GPIO_SPEED_LOW,
        .pull        = BSP_GPIO_PUPD_NONE,
        .alternate_function = BSP_GPIO_AF0
    };

	BSP_GPIO_Init(GPIOA, 5, &config_output);
	BSP_GPIO_Init(GPIOA, 8, &config_input);
	uint8_t input_state = 0;
	while(1) {
		BSP_GPIO_ReadPin(GPIOA, 8, &input_state);
		if (input_state == 1) {
			BSP_GPIO_SetPin(GPIOA, 5);
		}
		else {
			BSP_GPIO_ResetPin(GPIOA, 5);
		}
	}

}

/**
 * @brief  Validate BSP_GPIO_DeInit() in nominal cases
 *
 * @note  Debug validation :
 * 			Set breakpoint on BSP_GPIO_DeInit()
 * 			Watch variable : status (status = BSP_GPIO_OK)
 * 			Watch register values : each port register in SFRs view
 * 			Expected register values after BSP_GPIO_Init() call (for GPIOA 5 pin and GPIOA 8)
 *
 * 			 config0  -> MODER  bits [11:10] = 0b00  (INPUT)
 *             			 OTYPER bit  [5]     = 0b0   (PUSH_PULL)
 *             			 OSPEEDR bits[11:10] = 0b00  (LOW)
 *             			 PUPDR  bits [11:10] = 0b00  (NONE)
 *
 */
void test_bsp_gpio_deinit(void) {
	BSP_GPIO_Status_t status;
	BSP_GPIO_Config_t config0 = {
	    .mode        = BSP_GPIO_MODE_OUTPUT,
	    .output_type = BSP_GPIO_OTYPE_OPEN_DRAIN,
	    .speed       = BSP_GPIO_SPEED_HIGH,
	    .pull        = BSP_GPIO_PUPD_PULL_DOWN,
	    .alternate_function = BSP_GPIO_AF0
	};

	status = BSP_GPIO_Init(GPIOA, 5, &config0);
	status = BSP_GPIO_DeInit(GPIOA, 5);
	__NOP();
}

/**
 * @brief  Validate BSP_GPIO_EXTI_Config() nominal cases
 *
 * @note   Hardware setup : none required
 *           All validations are done by reading hardware registers in debug mode
 *
 * @note   How to validate — set a breakpoint after each BSP_GPIO_EXTI_Config() call
 *           and verify the following registers in the SFRs debug view :
 *
*           After config0 — GPIOA pin 5, RISING, priority 3 :
 *             RCC->APB2ENR  bit 14 (SYSCFGEN) = 0x1    — SYSCFG clock enabled
 *             RCC->APB2ENR  bit 15 (EXTITEN)  = 0x1    — EXTI clock enabled
 *             SYSCFG->EXTICR[1] bits [7:4]    = 0x0  — pin5 → EXTICR[1], pos (5%4)*4=4 → bits[7:4], GPIOA
 *             EXTI->RTSR    bit 5             = 0x1  — rising trigger enabled
 *             EXTI->FTSR    bit 5             = 0x0  — falling trigger disabled
 *             NVIC->IPR[5]  bits [31:24]      = 0x30 — priority 3 for EXTI9_5_IRQn (IRQn=23)
 *             NVIC->ISER[0] bit 23            = 0x1  — EXTI9_5_IRQn enabled
 *
 *           After config1 — GPIOA pin 5, FALLING, priority 5 :
 *             EXTI->RTSR    bit 5             = 0x0  — rising trigger disabled
 *             EXTI->FTSR    bit 5             = 0x1  — falling trigger enabled
 *             NVIC->IPR[5]  bits [31:24]      = 0x50 — priority updated
 *
 *           After config2 — GPIOA pin 5, BOTH, priority 0 :
 *             EXTI->RTSR    bit 5             = 0x1  — rising trigger enabled
 *             EXTI->FTSR    bit 5             = 0x1  — falling trigger enabled
 *             NVIC->IPR[5]  bits [31:24]      = 0x00 — highest priority
 *
 *           After config3 — GPIOC pin 13, RISING, priority 10 :
 *             SYSCFG->EXTICR[3] bits [7:4]    = 0x2  — pin13 → EXTICR[3], pos (13%4)*4=4 → bits[7:4], GPIOC
 *             EXTI->RTSR    bit 13            = 0x1  — rising trigger enabled
 *             EXTI->FTSR    bit 13            = 0x0  — falling trigger disabled
 *             NVIC->IPR[10] bits [7:0]        = 0xA0 — priority 10 for EXTI15_10_IRQn (IRQn=40)
 *             NVIC->ISER[1] bit 8             = 0x1  — EXTI15_10_IRQn enabled (IRQn=40, 40-32=8)
 *
 *           After config4 — GPIOA pin 0, RISING, priority 1 :
 *             SYSCFG->EXTICR[0] bits [3:0]    = 0x0  — pin0 → EXTICR[0], pos 0, GPIOA
 *             EXTI->RTSR    bit 0             = 0x1  — rising trigger enabled
 *             NVIC->IPR[1]  bits [23:16]      = 0x10 — priority 1 for EXTI0_IRQn (IRQn=6)
 *             NVIC->ISER[0] bit 6             = 0x1  — EXTI0_IRQn enabled
 *
 *           After config5 — GPIOA pin 9, FALLING, priority 7 :
 *             SYSCFG->EXTICR[2] bits [7:4]    = 0x0  — pin9 → EXTICR[2], pos (9%4)*4=4 → bits[7:4], GPIOA
 *             EXTI->RTSR    bit 9             = 0x0  — rising trigger disabled
 *             EXTI->FTSR    bit 9             = 0x1  — falling trigger enabled
 *             NVIC->IPR[5]  bits [31:24]      = 0x70 — priority 7 for EXTI9_5_IRQn (IRQn=23), shared with pin5
 *             NVIC->ISER[0] bit 23            = 0x1  — EXTI9_5_IRQn enabled
 *
 *           After config6 — GPIOH pin 1, RISING, priority 2 :
 *             SYSCFG->EXTICR[0] bits [7:4]    = 0x7  — pin1 → EXTICR[0], pos (1%4)*4=4 → bits[7:4], GPIOH
 *             EXTI->RTSR    bit 1             = 0x1  — rising trigger enabled
 *             NVIC->IPR[1]  bits [31:24]      = 0x20 — priority 2 for EXTI1_IRQn (IRQn=7)
 *             NVIC->ISER[0] bit 7             = 0x1  — EXTI1_IRQn enabled
 *
 * @note   NVIC->IP index and NVIC->ISER bit calculation :
 *           EXTI0_IRQn     = 6  → ISER[0] bit 6,  IP[6]
 *           EXTI1_IRQn     = 7  → ISER[0] bit 7,  IP[7]
 *           EXTI2_IRQn     = 8  → ISER[0] bit 8,  IP[8]
 *           EXTI3_IRQn     = 9  → ISER[0] bit 9,  IP[9]
 *           EXTI4_IRQn     = 10 → ISER[0] bit 10, IP[10]
 *           EXTI9_5_IRQn   = 23 → ISER[0] bit 23, IP[23]
 *           EXTI15_10_IRQn = 40 → ISER[1] bit 8,  IP[40]
 *
 * @note   NVIC->IP priority bits : on STM32F410, __NVIC_PRIO_BITS = 4
 *           Priority is stored in bits [7:4] of IP[n]
 *           Value written = priority << (8 - __NVIC_PRIO_BITS) = priority << 4
 *           Example : priority 3 → IP[n] bits[7:4] = 3 → raw value = 0x30
 *
 * @note   Dependencies : none — BSP_GPIO_EXTI_Config() handles all configuration
 *           including clock enables
 */
void test_bsp_gpio_it_config_nominal(void) {

    BSP_GPIO_Status_t status;

    /* config0 : GPIOA pin 5, RISING trigger, priority 3
     * Tests : RISING trigger path, EXTI9_5 IRQn group, GPIOA port mapping,
     *         pin in EXTICR[1] */
    status = BSP_GPIO_EXTI_Config(GPIOA, 5, BSP_GPIO_IT_RISING, 3);
    /* Expected : BSP_GPIO_OK */

    /* config1 : GPIOA pin 5, FALLING trigger, priority 5
     * Tests : FALLING trigger path, RTSR cleared when switching trigger */
    status = BSP_GPIO_EXTI_Config(GPIOA, 5, BSP_GPIO_IT_FALLING, 5);
    /* Expected : BSP_GPIO_OK */

    /* config2 : GPIOA pin 5, BOTH triggers, priority 0
     * Tests : BOTH trigger path — RTSR and FTSR both set */
    status = BSP_GPIO_EXTI_Config(GPIOA, 5, BSP_GPIO_IT_BOTH, 0);
    /* Expected : BSP_GPIO_OK */

    /* config3 : GPIOC pin 13, RISING trigger, priority 10
     * Tests : GPIOC port mapping (cr_port_val = 2),
     *         EXTI15_10 IRQn group, EXTICR[3] register,
     *         NVIC->ISER[1] (IRQn >= 32) */
    status = BSP_GPIO_EXTI_Config(GPIOC, 13, BSP_GPIO_IT_RISING, 10);
    /* Expected : BSP_GPIO_OK */

    /* config4 : GPIOA pin 0, RISING trigger, priority 1
     * Tests : EXTI0 IRQn (individual handler), EXTICR[0] register,
     *         lowest pin boundary */
    status = BSP_GPIO_EXTI_Config(GPIOA, 0, BSP_GPIO_IT_RISING, 1);
    /* Expected : BSP_GPIO_OK */

    /* config5 : GPIOA pin 9, FALLING trigger, priority 7
     * Tests : pin 9 in EXTICR[2], EXTI9_5 IRQn shared with pin 5
     *         priority update on shared IRQn */
    status = BSP_GPIO_EXTI_Config(GPIOA, 9, BSP_GPIO_IT_FALLING, 7);
    /* Expected : BSP_GPIO_OK */

    /* config6 : GPIOH pin 1, RISING trigger, priority 2
     * Tests : GPIOH port mapping (cr_port_val = 7),
     *         EXTI1 IRQn, EXTICR[0] bits [7:4] */
    status = BSP_GPIO_EXTI_Config(GPIOH, 1, BSP_GPIO_IT_RISING, 2);
    /* Expected : BSP_GPIO_OK */

    /* config7 : GPIOA pin 15, BOTH triggers, priority 15
     * Tests : highest pin boundary, EXTI15_10 IRQn,
     *         EXTICR[3] bits [15:12], maximum priority value */
    status = BSP_GPIO_EXTI_Config(GPIOA, 15, BSP_GPIO_IT_BOTH, 15);
    /* Expected : BSP_GPIO_OK */
}


