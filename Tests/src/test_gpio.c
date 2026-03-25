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

void test_gpio_config(void) {
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

	GPIO_Init(GPIOA, 5, &config0);
	GPIO_Init(GPIOA, 5, &config1);
	GPIO_Init(GPIOA, 5, &config2);
	GPIO_Init(GPIOA, 5, &config3);
	GPIO_Init(GPIOA, 5, &config4);
	GPIO_Init(GPIOA, 5, &config5);
	GPIO_Init(GPIOA, 5, &config6);
	GPIO_Init(GPIOA, 5, &config7);
	GPIO_Init(GPIOA, 5, &config8);
	GPIO_Init(GPIOA, 5, &config9);
	GPIO_Init(GPIOA, 8, &config10);

	/*
	 * Expected register values after GPIO_Init() call (for GPIOA 5 pin and GPIOA 8)
	 *
	 * config0  -> MODER  bits [11:10] = 0b01  (OUTPUT)
	 *             OTYPER bit  [5]     = 0b0   (PUSH_PULL)
	 *             OSPEEDR bits[11:10] = 0b00  (LOW)
	 *             PUPDR  bits [11:10] = 0b00  (NONE)
	 *
	 * config1  -> MODER  bits [11:10] = 0b00  (INPUT)
	 *             PUPDR  bits [11:10] = 0b00  (NONE)
	 *
	 * config2  -> MODER  bits [11:10] = 0b11  (ANALOG)
	 *
	 * config3  -> MODER  bits [11:10] = 0b10  (AF)
	 *             OTYPER bit  [5]     = 0b0   (PUSH_PULL)
	 *             OSPEEDR bits[11:10] = 0b00  (LOW)
	 *             AFR[0] bits [23:20] = 0x7   (AF7) for pin 5
	 *
	 * config4  -> OTYPER bit  [5]     = 0b1   (OPEN_DRAIN)
	 *
	 * config5  -> OSPEEDR bits[11:10] = 0b01  (MEDIUM)
	 *
	 * config6  -> OSPEEDR bits[11:10] = 0b10  (HIGH)
	 *
	 * config7  -> OSPEEDR bits[11:10] = 0b11  (VERY_HIGH)
	 *
	 * config8  -> PUPDR  bits [11:10] = 0b01  (PULL_UP)
	 *
	 * config9  -> PUPDR  bits [11:10] = 0b10  (PULL_DOWN)
	 *
	 * config10 -> MODER  bits [17:16] = 0b10  (AF)        for pin 8
	 *             AFR[1] bits [3:0]   = 0x7   (AF7)       for pin 8
	 */
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


