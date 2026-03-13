/*
 * gpio.h
 *
 *  Created on: Mar 13, 2026
 *      Author: DANGUIAT
 */

#ifndef BSP_GPIO_GPIO_H_
#define BSP_GPIO_GPIO_H_

#include <stdint.h>
#include "stm32f4xx.h"

/*
 * GPIO Mode (RM0401 - Section 6.4.1 - GPIOx_MODER)
 * Bits 2y:2y+1 MODERy[1:0] - Port x configuration bits (y = 0..15)
 */
typedef enum {
    BSP_GPIO_MODE_INPUT  = 0x00,    // MODER = 0b00 - Input mode
    BSP_GPIO_MODE_OUTPUT = 0x01,    // MODER = 0b01 - General purpose output
    BSP_GPIO_MODE_AF     = 0x02,    // MODER = 0b10 - Alternate function
    BSP_GPIO_MODE_ANALOG = 0x03     // MODER = 0b11 - Analog mode
} GPIO_Mode_t;

/*
 * GPIO Output Type (RM0401 - Section 6.4.2 - GPIOx_OTYPER)
 * Bit y OTy - Port x configuration bit (y = 0..15)
 */
typedef enum {
    BSP_GPIO_OTYPE_PUSH_PULL  = 0x00,   // OTYPER = 0 - Output push-pull
    BSP_GPIO_OTYPE_OPEN_DRAIN = 0x01    // OTYPER = 1 - Output open-drain
} GPIO_OType_t;

/*
 * GPIO Output Speed (RM0401 - Section 6.4.3 - GPIOx_OSPEEDR)
 * Bits 2y:2y+1 OSPEEDRy[1:0] - Port x configuration bits (y = 0..15)
 */
typedef enum {
    BSP_GPIO_SPEED_LOW       = 0x00,    // OSPEEDR = 0b00 - Low speed (~2 MHz)
    BSP_GPIO_SPEED_MEDIUM    = 0x01,    // OSPEEDR = 0b01 - Medium speed (~25 MHz)
    BSP_GPIO_SPEED_HIGH      = 0x02,    // OSPEEDR = 0b10 - High speed (~50 MHz)
    BSP_GPIO_SPEED_VERY_HIGH = 0x03     // OSPEEDR = 0b11 - Very high speed (~100 MHz)
} GPIO_Speed_t;

/*
 * GPIO Pull-up / Pull-down (RM0401 - Section 6.4.4 - GPIOx_PUPDR)
 * Bits 2y:2y+1 PUPDRy[1:0] - Port x configuration bits (y = 0..15)
 */
typedef enum {
    BSP_GPIO_PUPD_NONE      = 0x00,     // PUPDR = 0b00 - No pull-up, pull-down
    BSP_GPIO_PUPD_PULL_UP   = 0x01,     // PUPDR = 0b01 - Pull-up
    BSP_GPIO_PUPD_PULL_DOWN = 0x02,     // PUPDR = 0b10 - Pull-down
    BSP_GPIO_PUPD_RESERVED  = 0x03      // PUPDR = 0b11 - Reserved
} GPIO_PuPd_t;

/*
 * GPIO Alternate Functions (RM0401 - Section 6.4.9/6.4.10 - GPIOx_AFRL/AFRH)
 * Bits AFRLy[3:0] / AFRHy[3:0] - Alternate function selection (y = 0..15)
 * AF mapping per pin defined in STM32F410RB datasheet - Table 9
 */
typedef enum {
    BSP_GPIO_AF0  = 0x00,   // SYS_AF    : JTCK/SWCLK, JTDI, JTDO, JTMS/SWDIO,
                            //             JTRST, RTC_REFIN, MCO1, MCO2
    BSP_GPIO_AF1  = 0x01,   // TIM1/TIM2 : TIM1_CH1..4, TIM1_ETR, TIM1_BKIN,
                            //             TIM2_CH1..4, TIM2_ETR
    BSP_GPIO_AF2  = 0x02,   // TIM3/TIM5 : TIM3_CH1..4, TIM3_ETR,
                            //             TIM5_CH1..4
    BSP_GPIO_AF3  = 0x03,   // TIM9/TIM11: TIM9_CH1..2, TIM11_CH1
    BSP_GPIO_AF4  = 0x04,   // I2C1/I2C2 : I2C1_SCL, I2C1_SDA, I2C1_SMBA,
                            //             I2C2_SCL, I2C2_SDA, I2C2_SMBA
    BSP_GPIO_AF5  = 0x05,   // SPI1/SPI2 : SPI1_SCK, SPI1_MISO, SPI1_MOSI, SPI1_NSS,
                            //             SPI2_SCK, SPI2_MISO, SPI2_MOSI, SPI2_NSS
    BSP_GPIO_AF6  = 0x06,   // SPI3/I2S3 : SPI3_SCK, SPI3_MISO, SPI3_MOSI, SPI3_NSS
    BSP_GPIO_AF7  = 0x07,   // USART1/2  : USART1_TX, USART1_RX, USART1_CK,
                            //             USART1_CTS, USART1_RTS,
                            //             USART2_TX, USART2_RX, USART2_CK,
                            //             USART2_CTS, USART2_RTS
    BSP_GPIO_AF8  = 0x08,   // USART6    : USART6_TX, USART6_RX, USART6_CK
    BSP_GPIO_AF9  = 0x09,   // I2C2/FMPI2C1 : I2C2_SCL, I2C2_SDA,
                            //                FMPI2C1_SCL, FMPI2C1_SDA,
                            //                FMPI2C1_SMBA
    BSP_GPIO_AF10 = 0x0A,   // OTG_FS    : OTG_FS_SOF
    BSP_GPIO_AF11 = 0x0B,   // (réservé sur STM32F410RB)
    BSP_GPIO_AF12 = 0x0C,   // (réservé sur STM32F410RB)
    BSP_GPIO_AF13 = 0x0D,   // (réservé sur STM32F410RB)
    BSP_GPIO_AF14 = 0x0E,   // (réservé sur STM32F410RB)
    BSP_GPIO_AF15 = 0x0F    // EVENTOUT  : Cortex EVENTOUT signal
} GPIO_AF_t;

/*
 * GPIO Status codes - return type for all BSP GPIO functions
 */
typedef enum {
    BSP_GPIO_OK      = 0x00,    // Operation successful
    BSP_GPIO_ERROR   = 0x01,    // Generic hardware error
    BSP_GPIO_INVALID = 0x02,    // Invalid parameter passed by caller
    BSP_GPIO_BUSY    = 0x03,    // Resource currently in use
    BSP_GPIO_TIMEOUT = 0x04,    // Operation timed out
    BSP_GPIO_NOT_INIT = 0x05    // Pin not initialized before use
} GPIO_Status_t;

/*
 * GPIO Configuration structure
 * Used as parameter for GPIO_Init()
 */
typedef struct {
    GPIO_Mode_t     mode;               // Pin mode (input/output/AF/analog)
    GPIO_OType_t    output_type;        // Output type (push-pull/open-drain)
    GPIO_Speed_t    speed;              // Output speed
    GPIO_PuPd_t     pull;               // Pull-up/pull-down configuration
    GPIO_AF_t       alternate_function; // Alternate function selection (AF mode only)
} GPIO_Config_t;

/*
 * BSP GPIO Function prototypes
 */
GPIO_Status_t GPIO_Init(GPIO_TypeDef *port, uint8_t pin, GPIO_Config_t *config);
GPIO_Status_t GPIO_SetPin(GPIO_TypeDef *port, uint8_t pin);
GPIO_Status_t GPIO_ResetPin(GPIO_TypeDef *port, uint8_t pin);
GPIO_Status_t GPIO_TogglePin(GPIO_TypeDef *port, uint8_t pin);
uint8_t       GPIO_ReadPin(GPIO_TypeDef *port, uint8_t pin);


#endif /* BSP_GPIO_GPIO_H_ */
