---
description: Tests unitaires host-side Unity (logique pure, sur PC) puis analyse statique cppcheck MISRA du code BSP.
argument-hint: "[driver]  ex: gpio, uart — vide = tous"
---

# /test — Tests host-side + analyse statique

## Périmètre
Argument `$ARGUMENTS` : nom du driver (gpio, uart...). Vide = tous les drivers.

## Partie 1 — Tests Unity host-side
Les tests host-side ne testent que la logique pure (calculs, masques,
machines d'état, validation de paramètres). Ils ne doivent jamais accéder
à une adresse de registre réelle : sur PC, cela provoque un crash.

1. Lister les fichiers `Tests/host/test_<driver>*.c`
   - Si aucun fichier n'existe : le signaler et proposer une structure,
     sans rien créer sans validation
2. Compiler chaque suite avec GCC natif :
   `gcc -std=c11 -Wall -Wextra -Ivendor/Unity/src -IDrivers/BSP/<driver> vendor/Unity/src/unity.c Tests/host/test_<driver>.c -o build/host/test_<driver>.exe`
3. Exécuter chaque binaire et collecter les résultats

## Partie 2 — Analyse statique cppcheck
Sur `Drivers/BSP/<driver>/` (ou tout `Drivers/BSP/` si pas d'argument) :
`cppcheck --enable=warning,style,performance,portability --std=c11 --addon=misra --suppress=missingIncludeSystem --inline-suppr Drivers/BSP/<driver>/`

## Format du rapport
```
TESTS HOST  : N passés / N échoués / N ignorés
CPPCHECK    : N problèmes (dont N violations MISRA)
```
Pour chaque échec : test concerné, valeur attendue, valeur obtenue, cause probable.
Pour chaque violation MISRA : règle, fichier:ligne, explication courte.

## Règles
- Ne jamais modifier un test pour le faire passer
- Ne jamais corriger le code sans validation
- Les dérogations MISRA déjà documentées (ex : Rule 15.5 early return) sont
  signalées comme telles, pas comme des erreurs
