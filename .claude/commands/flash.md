---
description: Flash du binaire sur la NUCLEO-F410RB via STM32_Programmer_CLI (SWD). Invocation manuelle uniquement, confirmation obligatoire.
disable-model-invocation: true
---

# /flash — Programmation de la carte

Cette commande n'est jamais déclenchée par l'agent de sa propre initiative.

## Prérequis
- `STM32_Programmer_CLI` dans le PATH
- Un build récent et réussi (`/build`) — sinon proposer de lancer `/build` d'abord
- Carte NUCLEO-F410RB connectée en USB (ST-LINK)
- Aucune session de debug CubeIDE ouverte (elle occupe le ST-LINK)

## Étapes
1. Identifier le fichier `Debug/*.elf` et afficher sa date de modification
2. Vérifier que l'.elf est plus récent que les derniers fichiers modifiés
   dans Drivers/BSP/ et Core/ — sinon avertir l'utilisateur
3. Afficher la commande exacte et attendre la confirmation explicite :
   `STM32_Programmer_CLI -c port=SWD -w Debug/<projet>.elf -v -rst`
   - `-c port=SWD` : connexion via le ST-LINK en SWD
   - `-w` : écriture du binaire
   - `-v` : vérification après écriture
   - `-rst` : reset de la carte après programmation
4. Analyser la sortie : succès de l'écriture, succès de la vérification

## En cas d'échec
- « No ST-LINK detected » : câble, driver ST-LINK, ou session debug ouverte
- Échec de vérification : proposer de relancer, ne pas insister au-delà
- Ne jamais tenter d'effacement complet (mass erase) sans demande explicite

## Après le flash
Rappeler le protocole de test hardware attendu (quoi observer sur la carte)
si un test hardware est en cours.
