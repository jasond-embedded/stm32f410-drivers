---
description: Registres GPIO, EXTI, NVIC et SYSCFG du STM32F410RB. Charger pour toute implémentation ou debug impliquant les périphériques GPIO, interruptions externes ou configuration des pins.
paths:
  - "Drivers/BSP/gpio/**"
  - "Tests/src/test_gpio*"
  - "Tests/inc/test_gpio*"
---

# Registres GPIO — STM32F410RB (RM0401 Rev4)

## Adresses de base

```
GPIOA_BASE  = 0x40020000
GPIOB_BASE  = 0x40020400
GPIOC_BASE  = 0x40020800
GPIOH_BASE  = 0x40021C00
RCC_BASE    = 0x40023800
SYSCFG_BASE = 0x40013800
EXTI_BASE   = 0x40013C00
```

## RCC — Activation des horloges GPIO
### RCC_AHB1ENR (offset 0x30)

```
bit 0  GPIOAEN  → horloge GPIOA
bit 1  GPIOBEN  → horloge GPIOB
bit 2  GPIOCEN  → horloge GPIOC
bit 7  GPIOHEN  → horloge GPIOH
```

```c
/* Toujours activer avant tout accès aux registres GPIO */
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
/* NE PAS utiliser RCC->APB2LPENR — registre Low Power uniquement */
```

## Registres GPIO — offset depuis GPIOx_BASE

```
offset 0x00  MODER    mode input/output/AF/analog     2 bits/pin
offset 0x04  OTYPER   push-pull ou open-drain         1 bit/pin
offset 0x08  OSPEEDR  vitesse de sortie               2 bits/pin
offset 0x0C  PUPDR    pull-up / pull-down             2 bits/pin
offset 0x10  IDR      lecture état pins (read-only)   1 bit/pin
offset 0x14  ODR      écriture état pins              1 bit/pin
offset 0x18  BSRR     set/reset atomique              write-only
offset 0x1C  LCKR     verrouillage configuration
offset 0x20  AFR[0]   alternate function pins 0-7     4 bits/pin
offset 0x24  AFR[1]   alternate function pins 8-15    4 bits/pin
```

## MODER — GPIOx_MODER (RM0401 Section 6.4.1)

```
0b00 = BSP_GPIO_MODE_INPUT   = 0x00
0b01 = BSP_GPIO_MODE_OUTPUT  = 0x01
0b10 = BSP_GPIO_MODE_AF      = 0x02
0b11 = BSP_GPIO_MODE_ANALOG  = 0x03

Position : bits (2*pin+1):(2*pin) — 2 bits par pin
```

```c
port->MODER &= ~(0x3UL << (pin * 2U));
port->MODER |=  ((uint32_t)config->mode << (pin * 2U));
```

## OTYPER — GPIOx_OTYPER (RM0401 Section 6.4.2)

```
0 = BSP_GPIO_OTYPE_PUSH_PULL  = 0x00
1 = BSP_GPIO_OTYPE_OPEN_DRAIN = 0x01

Position : bit pin — 1 bit par pin
Applicable uniquement en mode OUTPUT et AF — ignoré en INPUT et ANALOG
```

```c
port->OTYPER &= ~(0x1UL << pin);
port->OTYPER |=  ((uint32_t)config->output_type << pin);
```

## OSPEEDR — GPIOx_OSPEEDR (RM0401 Section 6.4.3)

```
0b00 = BSP_GPIO_SPEED_LOW       = 0x00  (~2 MHz)
0b01 = BSP_GPIO_SPEED_MEDIUM    = 0x01  (~25 MHz)
0b10 = BSP_GPIO_SPEED_HIGH      = 0x02  (~50 MHz)
0b11 = BSP_GPIO_SPEED_VERY_HIGH = 0x03  (~100 MHz)

Position : bits (2*pin+1):(2*pin) — 2 bits par pin
Applicable uniquement en mode OUTPUT et AF
```

## PUPDR — GPIOx_PUPDR (RM0401 Section 6.4.4)

```
0b00 = BSP_GPIO_PUPD_NONE      = 0x00
0b01 = BSP_GPIO_PUPD_PULL_UP   = 0x01
0b10 = BSP_GPIO_PUPD_PULL_DOWN = 0x02
0b11 = BSP_GPIO_PUPD_RESERVED  = 0x03

Position : bits (2*pin+1):(2*pin) — 2 bits par pin
```

## BSRR — GPIOx_BSRR (RM0401 Section 6.4.7)

```
bits 15:0   BSy  set pin y   → écrire 1 pour forcer HIGH
bits 31:16  BRy  reset pin y → écrire 1 pour forcer LOW
Écrire 0 n'a aucun effet
Registre write-only — toujours atomique, une seule instruction
Si BS et BR simultanément à 1 → BS prime (HIGH)
```

```c
port->BSRR = (0x1UL << pin);           /* SET  — atomique */
port->BSRR = (0x1UL << (pin + 16U));   /* RESET — atomique */
/* Ne jamais utiliser ODR |= ou ODR &= — non atomique */
```

## AFR — GPIOx_AFRL/AFRH (RM0401 Sections 6.4.9 / 6.4.10)

```
AFR[0] = AFRL → pins 0 à 7   — 4 bits par pin
AFR[1] = AFRH → pins 8 à 15  — 4 bits par pin

Position dans AFRL : bits (4*pin+3):(4*pin)
Position dans AFRH : bits (4*(pin-8)+3):(4*(pin-8))

Valeurs AF0..AF15 → voir datasheet STM32F410RB Table 9
AF4 = I2C1/2
AF5 = SPI1/2
AF6 = SPI3
AF7 = USART1/2
```

```c
if (pin <= 7U) {
    port->AFR[0] &= ~(0xFUL << (pin * 4U));
    port->AFR[0] |=  ((uint32_t)config->alternate_function << (pin * 4U));
} else {
    port->AFR[1] &= ~(0xFUL << ((pin - 8U) * 4U));
    port->AFR[1] |=  ((uint32_t)config->alternate_function << ((pin - 8U) * 4U));
}
```

## LCKR — Séquence de verrouillage (RM0401 Section 6.4.8)

```
bit 16    LCKK   clé de verrouillage
bits 15:0 LCKy   verrouille la configuration de la pin y
```

```c
/* Séquence obligatoire — LCKR[15:0] ne doit pas changer */
__IO uint32_t tmp = (0x1UL << 16U) | (0x1UL << pin);
port->LCKR = tmp;             /* WR LCKK=1 + LCKy=1 */
port->LCKR = (0x1UL << pin);  /* WR LCKK=0 + LCKy=1 */
port->LCKR = tmp;             /* WR LCKK=1 + LCKy=1 */
tmp = port->LCKR;             /* RD obligatoire       */
/* Vérifier bit 16 = 1 pour confirmer le lock */
```

## SYSCFG — Mapping EXTI (RM0401 Section 7.2)
### RCC_APB2ENR — Activation SYSCFG et EXTI

```
bit 14  SYSCFGEN  → horloge SYSCFG
bit 15  EXTITEN   → horloge EXTI
```

```c
RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
RCC->APB2ENR |= RCC_APB2ENR_EXTITEN;
/* NE PAS utiliser RCC_APB2LPENR */
```

### SYSCFG_EXTICRx (offset 0x08 à 0x14)

```
EXTICR[0] → pins 0-3   (4 bits par pin)
EXTICR[1] → pins 4-7
EXTICR[2] → pins 8-11
EXTICR[3] → pins 12-15

Index    : pin >> 2U
Position : (pin % 4U) << 2U

Valeur du champ :
0x0 = GPIOA
0x1 = GPIOB
0x2 = GPIOC
0x7 = GPIOH
```

```c
uint8_t exti_idx = pin >> 2U;
uint8_t exti_pos = (pin % 4U) << 2U;
SYSCFG->EXTICR[exti_idx] &= ~(0xFUL << exti_pos);
SYSCFG->EXTICR[exti_idx] |=  ((uint32_t)cr_port_val << exti_pos);
```

## EXTI — Registres d'interruption (RM0401 Section 9.3)

```
IMR   offset 0x00  masque interruption    1=activé  1 bit/ligne
EMR   offset 0x04  masque événement
RTSR  offset 0x08  trigger montant        1=activé
FTSR  offset 0x0C  trigger descendant     1=activé
PR    offset 0x14  pending register       écrire 1 pour clearer
```

```c
EXTI->IMR  |=  (0x1UL << pin);  /* activer ligne */
EXTI->IMR  &= ~(0x1UL << pin);  /* désactiver ligne */
EXTI->RTSR |=  (0x1UL << pin);  /* trigger montant */
EXTI->FTSR |=  (0x1UL << pin);  /* trigger descendant */
EXTI->PR    =  (0x1UL << pin);  /* clear pending — écriture directe */
```

## NVIC — Lignes EXTI vers IRQn (PM0214)

```
EXTI0_IRQn      = 6   → pin 0 uniquement
EXTI1_IRQn      = 7   → pin 1 uniquement
EXTI2_IRQn      = 8   → pin 2 uniquement
EXTI3_IRQn      = 9   → pin 3 uniquement
EXTI4_IRQn      = 10  → pin 4 uniquement
EXTI9_5_IRQn    = 23  → pins 5 à 9 partagées
EXTI15_10_IRQn  = 40  → pins 10 à 15 partagées

STM32F410 : 4 bits de priorité → valeurs 0 à 15
NVIC->ISER[0] pour IRQn < 32
NVIC->ISER[1] pour IRQn >= 32 (ex: EXTI15_10_IRQn=40 → ISER[1] bit 8)
```

```c
NVIC_SetPriority(exti_irqn, priority);
NVIC_EnableIRQ(exti_irqn);
```

## Nucleo-F410RB — Pins utiles

```
PA5  → LED LD2 (verte)   HIGH=allumée
PC13 → Bouton B1         actif LOW (pull-up interne carte)
```
