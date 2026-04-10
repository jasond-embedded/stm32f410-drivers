/*
 * test_gpio.c
 *
 *  Created on: Mar 13, 2026
 *      Author: DANGUIAT
 */

#include "test_gpio.h"
#include "gpio.h"

/**
 * @brief  Validate GPIO_Init() for nominal cases
 *
 * @note   Debug validation:
 *           Set breakpoints on each GPIO_Init() call
 *           Watch variables : status = BSP_GPIO_OK
 *           Watch register values : each port register in SFRs view
 * 			 Expected register values after GPIO_Init() call (for GPIOA 5 pin and GPIOA 8)
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
void test_gpio_init_nominal(void) {
	GPIO_Status_t status;
	/* --- MODER --- */
	/* config0 : OUTPUT - all MODER values covered by configs below */
	GPIO_Config_t config0 = {
	    .mode        = BSP_GPIO_MODE_OUTPUT,
	    .output_type = BSP_GPIO_OTYPE_PUSH_PULL,
	    .speed       = BSP_GPIO_SPEED_LOW,
	    .pull        = BSP_GPIO_PUPD_NONE,
	    .alternate_function = BSP_GPIO_AF0
	};

	GPIO_Config_t config1 = {
	    .mode        = BSP_GPIO_MODE_INPUT,
	    .output_type = BSP_GPIO_OTYPE_PUSH_PULL,  /* ignored in INPUT mode */
	    .speed       = BSP_GPIO_SPEED_LOW,         /* ignored in INPUT mode */
	    .pull        = BSP_GPIO_PUPD_NONE,
	    .alternate_function = BSP_GPIO_AF0
	};

	GPIO_Config_t config2 = {
	    .mode        = BSP_GPIO_MODE_ANALOG,
	    .output_type = BSP_GPIO_OTYPE_PUSH_PULL,  /* ignored in ANALOG mode */
	    .speed       = BSP_GPIO_SPEED_LOW,         /* ignored in ANALOG mode */
	    .pull        = BSP_GPIO_PUPD_NONE,
	    .alternate_function = BSP_GPIO_AF0
	};

	GPIO_Config_t config3 = {
	    .mode        = BSP_GPIO_MODE_AF,
	    .output_type = BSP_GPIO_OTYPE_PUSH_PULL,
	    .speed       = BSP_GPIO_SPEED_LOW,
	    .pull        = BSP_GPIO_PUPD_NONE,
	    .alternate_function = BSP_GPIO_AF7        /* e.g. USART */
	};

	/* --- OTYPER (OUTPUT and AF modes only) --- */
	/* config0 already covers PUSH_PULL */
	GPIO_Config_t config4 = {
	    .mode        = BSP_GPIO_MODE_OUTPUT,
	    .output_type = BSP_GPIO_OTYPE_OPEN_DRAIN,
	    .speed       = BSP_GPIO_SPEED_LOW,
	    .pull        = BSP_GPIO_PUPD_NONE,
	    .alternate_function = BSP_GPIO_AF0
	};

	/* --- OSPEEDR (OUTPUT and AF modes only) --- */
	/* config0 already covers SPEED_LOW */
	GPIO_Config_t config5 = {
	    .mode        = BSP_GPIO_MODE_OUTPUT,
	    .output_type = BSP_GPIO_OTYPE_PUSH_PULL,
	    .speed       = BSP_GPIO_SPEED_MEDIUM,
	    .pull        = BSP_GPIO_PUPD_NONE,
	    .alternate_function = BSP_GPIO_AF0
	};

	GPIO_Config_t config6 = {
	    .mode        = BSP_GPIO_MODE_OUTPUT,
	    .output_type = BSP_GPIO_OTYPE_PUSH_PULL,
	    .speed       = BSP_GPIO_SPEED_HIGH,
	    .pull        = BSP_GPIO_PUPD_NONE,
	    .alternate_function = BSP_GPIO_AF0
	};

	GPIO_Config_t config7 = {
	    .mode        = BSP_GPIO_MODE_OUTPUT,
	    .output_type = BSP_GPIO_OTYPE_PUSH_PULL,
	    .speed       = BSP_GPIO_SPEED_VERY_HIGH,
	    .pull        = BSP_GPIO_PUPD_NONE,
	    .alternate_function = BSP_GPIO_AF0
	};

	/* --- PUPDR --- */
	/* config0 already covers PUPD_NONE */
	GPIO_Config_t config8 = {
	    .mode        = BSP_GPIO_MODE_INPUT,
	    .output_type = BSP_GPIO_OTYPE_PUSH_PULL,  /* ignored in INPUT mode */
	    .speed       = BSP_GPIO_SPEED_LOW,         /* ignored in INPUT mode */
	    .pull        = BSP_GPIO_PUPD_PULL_UP,
	    .alternate_function = BSP_GPIO_AF0
	};

	GPIO_Config_t config9 = {
	    .mode        = BSP_GPIO_MODE_INPUT,
	    .output_type = BSP_GPIO_OTYPE_PUSH_PULL,  /* ignored in INPUT mode */
	    .speed       = BSP_GPIO_SPEED_LOW,         /* ignored in INPUT mode */
	    .pull        = BSP_GPIO_PUPD_PULL_DOWN,
	    .alternate_function = BSP_GPIO_AF0
	};

	/* --- AFR (AF mode only) --- */
	/* config3 covers AF on pin <= 7 (AFRL) */
	/* config10 covers AF on pin >= 8 (AFRH) - same config, different pin */
	GPIO_Config_t config10 = {
	    .mode        = BSP_GPIO_MODE_AF,
	    .output_type = BSP_GPIO_OTYPE_PUSH_PULL,
	    .speed       = BSP_GPIO_SPEED_HIGH,
	    .pull        = BSP_GPIO_PUPD_NONE,
	    .alternate_function = BSP_GPIO_AF7        /* e.g. USART on pin >= 8 */
	};

	status = GPIO_Init(GPIOA, 5, &config1);
	status = GPIO_Init(GPIOA, 5, &config0);
	status = GPIO_Init(GPIOA, 5, &config2);
	status = GPIO_Init(GPIOA, 5, &config3);
	status = GPIO_Init(GPIOA, 5, &config4);
	status = GPIO_Init(GPIOA, 5, &config5);
	status = GPIO_Init(GPIOA, 5, &config6);
	status = GPIO_Init(GPIOA, 5, &config7);
	status = GPIO_Init(GPIOA, 5, &config8);
	status = GPIO_Init(GPIOA, 5, &config9);
	status = GPIO_Init(GPIOA, 8, &config10);
}

/**
 * @brief  Validate GPIO_Init() error handling and boundary cases
 *
 * @note   Debug validation:
 *           Set breakpoints on each GPIO_Init() call
 *           Watch variable : status
 *           All calls must return BSP_GPIO_ERROR or BSP_GPIO_INVALID
 *           None must return BSP_GPIO_OK
 *
 * @note   No hardware setup required — all cases return before
 *           accessing any register
 *
 */
void test_gpio_init_error_boundary_cases(void) {
    GPIO_Status_t status;

    GPIO_Config_t valid_config = {
        .mode               = BSP_GPIO_MODE_OUTPUT,
        .output_type        = BSP_GPIO_OTYPE_PUSH_PULL,
        .speed              = BSP_GPIO_SPEED_LOW,
        .pull               = BSP_GPIO_PUPD_NONE,
        .alternate_function = BSP_GPIO_AF0
    };

    /* --- BSP_GPIO_ERROR cases --- */

    /* port == NULL -> BSP_GPIO_ERROR */
    status = GPIO_Init(NULL, 5, &valid_config);
    /* Expected : BSP_GPIO_ERROR */

    /* --- BSP_GPIO_INVALID cases --- */

    /* pin > 15 -> BSP_GPIO_INVALID */
    status = GPIO_Init(GPIOA, 16, &valid_config);
    /* Expected : BSP_GPIO_INVALID */

    /* config == NULL -> BSP_GPIO_INVALID */
    status = GPIO_Init(GPIOA, 5, NULL);
    /* Expected : BSP_GPIO_INVALID */

    /* config.mode > 3, out of range -> BSP_GPIO_INVALID */
    GPIO_Config_t invalid_mode = {
        .mode               = 0x04,
        .output_type        = BSP_GPIO_OTYPE_PUSH_PULL,
        .speed              = BSP_GPIO_SPEED_LOW,
        .pull               = BSP_GPIO_PUPD_NONE,
        .alternate_function = BSP_GPIO_AF0
    };
    status = GPIO_Init(GPIOA, 5, &invalid_mode);
    /* Expected : BSP_GPIO_INVALID */

    /* config.output_type > 1, out of range -> BSP_GPIO_INVALID */
    GPIO_Config_t invalid_otype = {
        .mode               = BSP_GPIO_MODE_OUTPUT,
        .output_type        = 0x02,
        .speed              = BSP_GPIO_SPEED_LOW,
        .pull               = BSP_GPIO_PUPD_NONE,
        .alternate_function = BSP_GPIO_AF0
    };
    status = GPIO_Init(GPIOA, 5, &invalid_otype);
    /* Expected : BSP_GPIO_INVALID */

    /* config.speed >  out of range -> BSP_GPIO_INVALID */
    GPIO_Config_t invalid_speed = {
        .mode               = BSP_GPIO_MODE_OUTPUT,
        .output_type        = BSP_GPIO_OTYPE_PUSH_PULL,
        .speed              = 0x04,
        .pull               = BSP_GPIO_PUPD_NONE,
        .alternate_function = BSP_GPIO_AF0
    };
    status = GPIO_Init(GPIOA, 5, &invalid_speed);
    /* Expected : BSP_GPIO_INVALID */

    /* config.pull out of range -> BSP_GPIO_INVALID */
    GPIO_Config_t invalid_pull = {
        .mode               = BSP_GPIO_MODE_OUTPUT,
        .output_type        = BSP_GPIO_OTYPE_PUSH_PULL,
        .speed              = BSP_GPIO_SPEED_LOW,
        .pull               = 0x04,
        .alternate_function = BSP_GPIO_AF0
    };
    status = GPIO_Init(GPIOA, 5, &invalid_pull);
    /* Expected : BSP_GPIO_INVALID */

    /* config.alternate_function out of range -> BSP_GPIO_INVALID */
    GPIO_Config_t invalid_af = {
        .mode               = BSP_GPIO_MODE_OUTPUT,
        .output_type        = BSP_GPIO_OTYPE_PUSH_PULL,
        .speed              = BSP_GPIO_SPEED_LOW,
        .pull               = BSP_GPIO_PUPD_NONE,
        .alternate_function = 0x10
    };
    status = GPIO_Init(GPIOA, 5, &invalid_af);
    /* Expected : BSP_GPIO_INVALID */
}

/**
 * @brief  Validate GPIO_SetPin() by setting PA5 (LED) to logical high
 *
 * @note   Hardware setup:
 *           - PA5 : LED output (push-pull, no pull)
 *             Expected : LED turns on and stays on
 *
 * @note   Debug validation:
 *           Set breakpoint after GPIO_SetPin() call
 *           Register check : GPIOA->ODR bit [5] = 1
 *
 * @note   Dependencies: GPIO_Init() must be validated first
 */
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

/**
 * @brief  Validate GPIO_ResetPin() by blinking PA5 (LED) using
 *         GPIO_SetPin() and GPIO_ResetPin() alternately
 *
 * @note   Hardware setup:
 *           - PA5 : LED output (push-pull, no pull)
 *             Expected : LED blinks at 1Hz (500ms on, 500ms off)
 *
 * @note   Debug validation:
 *           Set breakpoint on GPIO_SetPin() and GPIO_ResetPin() calls
 *           Register check : GPIOA->ODR bit [5] toggles between 1 and 0
 *
 * @note   Dependencies: GPIO_Init() and GPIO_SetPin() must be validated first
 */
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
void test_gpio_toggle_pin_led_blink(void) {
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
    }
}

/**
 * @brief  Validate GPIO_ReadPin() by reading PA8 state and
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
 *           Set breakpoint on GPIO_ReadPin() call
 *           Watch variable : input_state
 *           Expected       : 1 when PA8 connected to 3V3
 *                            0 when PA8 connected to GND
 *           Register check : GPIOA->IDR bit [8] must match input_state
 *
 * @note   Dependencies: GPIO_Init() and GPIO_SetPin() must be validated first
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


