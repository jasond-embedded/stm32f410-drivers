---
description: Compilation croisée ARM du projet via le makefile généré par CubeIDE, puis rapport warnings/erreurs et taille du binaire.
argument-hint: "[clean]"
---

# /build — Compilation croisée ARM

## Prérequis
- Utiliser EXCLUSIVEMENT le make fourni par CubeIDE :
  `/c/ST/STM32CubeIDE_1.19.0/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.make.win32_2.2.0.202409170845/tools/bin/make.exe`
- Ne jamais utiliser le `make` du PATH (`/c/msys64/usr/bin/make`) : il transmet
  mal TMP/TEMP à arm-none-eabi-gcc (erreur "Cannot create temporary file in C:\WINDOWS\")
- `arm-none-eabi-gcc` et `arm-none-eabi-size` doivent être dans le PATH
- Le dossier `Debug/` et son makefile existent (générés par un premier build CubeIDE)
- Si `Debug/makefile` est absent ou si un nouveau fichier .c a été ajouté :
  s'arrêter et demander à l'utilisateur de lancer un build depuis CubeIDE

## Étapes
1. Vérifier la présence de `Debug/makefile`
2. Vérifier que le make.exe de CubeIDE existe au chemin ci-dessus.
   S'il est absent (mise à jour de CubeIDE) : s'arrêter, et demander à l'utilisateur
   de le localiser avec `find /c/ST -name make.exe` puis de mettre à jour ce fichier
   et CLAUDE.md. Ne jamais se rabattre sur le make du PATH.
3. Si l'argument est `clean` : proposer `"$CUBE_MAKE" -C Debug clean` avant le build
4. Proposer la commande suivante et attendre la validation :
   `"$CUBE_MAKE" -C Debug all > build.log 2>&1; echo "exit=$?"`
   (où CUBE_MAKE est le chemin complet ci-dessus, entre guillemets)
5. Lire `build.log` et analyser :
   - Classer chaque message : erreur / warning, avec fichier et ligne
   - Ignorer les warnings provenant de Drivers/CMSIS/ et Drivers/STM32F4xx_HAL_Driver/
   - Pour chaque warning dans Drivers/BSP/, Tests/ ou Core/ : expliquer la cause
     et la règle MISRA-C:2012 concernée si applicable
6. Si le build réussit (exit=0) : relever la taille affichée par
   `arm-none-eabi-size` en fin de log (ou la lancer sur `Debug/*.elf`)

## Format du rapport
```
BUILD : OK | ÉCHEC
Erreurs   : N
Warnings  : N (dont N dans le code projet : BSP / Tests / Core)
Taille    : text=... data=... bss=...
Flash     : text+data = ... / 131072 octets (..%)
RAM       : data+bss  = ... / 32768 octets (..%)
```
Puis la liste des problèmes du code projet, du plus grave au moins grave.
Les warnings d'une esquisse en cours de développement (ex. driver non terminé)
sont signalés comme tels, mais restent listés.

## Règles
- Ne jamais corriger le code automatiquement : proposer les corrections, attendre validation
- Un warning dans le code projet (BSP, Tests, Core) est traité comme une erreur (objectif : 0 warning)
- Une `implicit declaration of function` est toujours bloquante (MISRA-C:2012 Rule 17.3)