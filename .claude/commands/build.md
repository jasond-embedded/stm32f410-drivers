---
description: Compilation croisée ARM du projet via le makefile généré par CubeIDE, puis rapport warnings/erreurs et taille du binaire.
argument-hint: "[clean]"
---

# /build — Compilation croisée ARM

## Prérequis
- Outils dans le PATH : `make`, `arm-none-eabi-gcc`, `arm-none-eabi-size`
- Le dossier `Debug/` et son makefile existent (générés par un premier build CubeIDE)
- Si `Debug/makefile` est absent ou si un nouveau fichier .c a été ajouté :
  s'arrêter et demander à l'utilisateur de lancer un build depuis CubeIDE

## Étapes
1. Vérifier la présence de `Debug/makefile`
2. Si l'argument est `clean` : proposer `make -C Debug clean` avant le build
3. Proposer la commande `make -C Debug all 2>&1` et attendre la validation
4. Analyser la sortie :
   - Classer chaque message : erreur / warning, avec fichier et ligne
   - Ignorer les warnings provenant de Drivers/CMSIS/ et Drivers/STM32F4xx_HAL_Driver/
   - Pour chaque warning dans Drivers/BSP/ ou Tests/ : expliquer la cause
     et la règle MISRA-C:2012 concernée si applicable
5. Si le build réussit : lancer `arm-none-eabi-size Debug/*.elf`
   et présenter text / data / bss

## Format du rapport
```
BUILD : OK | ÉCHEC
Erreurs   : N
Warnings  : N (dont N dans le code BSP)
Taille    : text=... data=... bss=...  (Flash max 128 KB, RAM max 32 KB)
```
Puis la liste des problèmes du code BSP, du plus grave au moins grave.

## Règles
- Ne jamais corriger le code automatiquement : proposer les corrections, attendre validation
- Un warning dans le code BSP est traité comme une erreur (objectif : 0 warning)
