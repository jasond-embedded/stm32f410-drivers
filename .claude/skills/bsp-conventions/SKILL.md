---
description: Conventions de code, architecture et règles MISRA pour le projet BSP STM32F410. Charger pour toute écriture ou review de code dans Drivers/BSP/ ou Tests/.
paths:
  - "Drivers/BSP/**"
  - "Tests/**"
---

# BSP STM32F410 — Conventions et architecture

## Architecture en couches
```
Application (main.c)
    ↓
BSP Drivers (Drivers/BSP/<driver>/)
    ↓
CMSIS (stm32f410rx.h) — jamais réécrit
    ↓
Hardware (STM32F410RB)

Chaque couche ne communique qu'avec la couche directement en dessous.
BSP ne connaît pas l'application. CMSIS n'est jamais modifié.
```

## Structure des fichiers par driver
```
Drivers/BSP/<driver>/
<driver>.h ← interface publique uniquement
<driver>.c ← implémentation
Tests/
inc/test_<driver>.h
src/test_<driver>.c
```

## Nommage — obligatoire
```c
/* Préfixe BSP_ sur tous les types et fonctions publics */
BSP_GPIO_Status_t
BSP_GPIO_Config_t
BSP_GPIO_Init()

/* Valeurs d'enum préfixées — évite collisions avec HAL ST */
BSP_GPIO_MODE_INPUT
BSP_GPIO_OK

/* Types */
uint8_t  pour pin (0-15)
uint32_t pour les valeurs de registres
GPIO_TypeDef* pour les ports
```

## Opérations registres — obligatoire
```c
/* Toujours clear puis set — jamais de set sans clear préalable */
port->MODER &= ~((uint32_t)0x3U << (pin * 2U));
port->MODER |=  ((uint32_t)config->mode << (pin * 2U));

/* SetPin/ResetPin toujours via BSRR — jamais ODR direct */
port->BSRR = ((uint32_t)0x1U << pin);            /* set */
port->BSRR = ((uint32_t)0x1U << (pin + 16U));    /* reset */

/* Opérande gauche d'un shift : littéral U casté en uint32_t.
 * Amplitude de shift : suffixe U. Masques et comparaisons hors
 * shift : suffixe U seul (& 0xFFU, != 0U). */
((uint32_t)0x3U << (pin * 2U))   /* correct */
(0x3U << (pin * 2U))             /* incorrect — Rule 12.2 (voir ci-dessous) */
(0x3UL << (pin * 2U))            /* incorrect — UL interdit dans le code BSP */
(0x3 << (pin * 2))               /* incorrect — int signé : Rule 10.1,
                                    UB si résultat > INT_MAX */

/* Pourquoi le cast (uint32_t) :
 * Rule 12.2 → l'amplitude doit être < largeur du type ESSENTIEL de
 * l'opérande gauche. Le type essentiel d'une constante non signée est
 * le plus petit type qui la contient (UTLR) : 0x3U est essentiellement
 * unsigned char (8 bits), donc 0x3U << 30 viole 12.2.
 * (uint32_t)0x3U est essentiellement uint32_t (32 bits) : conforme.
 *
 * Pourquoi U et pas UL :
 * - cible Cortex-M4 : unsigned int = unsigned long = 32 bits
 * - tests host-side sur PC : unsigned long = 64 bits sous Linux/macOS
 *   (LP64), 32 bits sous Windows (LLP64). Avec (uint32_t)0x3U, le shift
 *   se fait en 32 bits sur la cible et sur tous les hôtes.
 * Les macros CMSIS (définies en UL) s'utilisent telles quelles :
 * fichiers vendor non modifiables, et U/UL sont tous deux
 * essentiellement non signés (Rule 10.4 respectée). */

/* Cast explicite sur les enums avant shift */
((uint32_t)config->mode << (pin * 2U))
```

## Validation des paramètres — obligatoire dans toutes les fonctions publiques
```c
if (port == NULL)                        return BSP_GPIO_ERROR;
if (pin > 15U)                           return BSP_GPIO_INVALID;
if (config == NULL)                      return BSP_GPIO_INVALID;
if (config->mode > BSP_GPIO_MODE_ANALOG) return BSP_GPIO_INVALID;
```

## Codes de retour
```c
typedef enum {
    BSP_<DRIVER>_OK       = 0x00,
    BSP_<DRIVER>_ERROR    = 0x01,  /* erreur hardware */
    BSP_<DRIVER>_INVALID  = 0x02,  /* paramètre invalide */
    BSP_<DRIVER>_BUSY     = 0x03,
    BSP_<DRIVER>_TIMEOUT  = 0x04,
    BSP_<DRIVER>_NOT_INIT = 0x05
} BSP_<DRIVER>_Status_t;
```

## Documentation Doxygen — obligatoire sur toutes les fonctions publiques
```c
/**
 * @brief  Description courte
 * @param  port    Description
 * @param  pin     Pin number (0 to 15)
 * @retval BSP_GPIO_OK      succès
 * @retval BSP_GPIO_ERROR   erreur hardware (NULL port)
 * @retval BSP_GPIO_INVALID paramètre invalide
 *
 * @note   Limitation ou précision importante
 *         Référence RM0401 si applicable
 */
```

## Commentaires registres — obligatoire
```c
/* Configure MODER - applies to all modes
 * RM0401 - Section 6.4.1 - GPIOx_MODER
 * Bits 2y:2y+1 MODERy[1:0] */
port->MODER &= ~((uint32_t)0x3U << (pin * 2U));
```

## MISRA-C:2012 — règles critiques pour ce projet
```
Rule 10.1 → shift et opérateurs bit à bit uniquement sur types non signés
Rule 10.3 → cast explicite avant shift sur enum
Rule 10.4 → opérandes de même type dans les expressions
Rule 12.2 → amplitude de shift < largeur du type essentiel de l'opérande
            gauche — un littéral U est essentiellement unsigned char :
            le caster en (uint32_t) avant de le décaler
Rule 14.4 → conditions de boucle booléennes explicites
Rule 15.5 → un seul return par fonction (advisory — dérogation documentée)
Rule 17.7 → vérifier les valeurs de retour des fonctions
Rule 3.1 → commentaires /* */ uniquement, pas de //
```

## Workflow de développement par driver
```
Lire RM0401 — section du périphérique concerné
Concevoir <driver>.h — enums, structs, prototypes, Doxygen
Implémenter <driver>.c — registres, validation, commentaires RM
Écrire test_<driver>.c — nominaux, limites, erreurs
Valider sur hardware — inspection SFR en debug
Commiter — feat → test → docs → chore
```