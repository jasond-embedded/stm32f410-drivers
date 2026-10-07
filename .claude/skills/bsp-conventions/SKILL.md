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
port->MODER &= ~(0x3UL << (pin * 2U));
port->MODER |=  ((uint32_t)config->mode << (pin * 2U));

/* SetPin/ResetPin toujours via BSRR — jamais ODR direct */
port->BSRR = (0x1UL << pin);           /* set */
port->BSRR = (0x1UL << (pin + 16U));   /* reset */

/* Suffixe UL sur les constantes utilisées dans les shifts */
(0x3UL << (pin * 2U))   /* correct */
(0x3 << (pin * 2))      /* incorrect */

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
port->MODER &= ~(0x3UL << (pin * 2U));
```

## MISRA-C:2012 — règles critiques pour ce projet
```
Rule 10.3 → cast explicite avant shift sur enum
Rule 10.4 → opérandes de même type dans les expressions
Rule 12.2 → shift uniquement sur types non signés (UL)
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