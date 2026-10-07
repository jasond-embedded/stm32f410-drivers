---
description: Génère le rapport de session (volet agentique + volet embarqué) dans docs/session_reports/.
---

# /report — Rapport de session

## Fichier
`docs/session_reports/YYYY-MM-DD.md` (date du jour).
Si le fichier existe déjà : ajouter une section `## Session N` à la suite.
Proposer le contenu avant écriture et attendre la validation.

## Structure du rapport

### 1. Résumé
Objectif de la session, mode d'apprentissage utilisé (A/B/C/D), résultat.

### 2. Volet embarqué
- Ce qui a été implémenté ou modifié (fichiers, fonctions)
- Choix techniques et leur justification, avec références RM0401
- Concepts nouveaux abordés
- Points restés flous : 2 à 3 questions de vérification pour l'utilisateur

### 3. Volet agentique
- Actions effectuées par l'agent (lectures, propositions, commandes)
- Actions proposées puis refusées ou modifiées par l'utilisateur, et pourquoi
- Skills chargés, commandes utilisées
- Ce qui aurait pu être plus efficace (prompts, découpage des tâches, contexte)
- Consommation de contexte : compactions survenues, informations perdues

### 4. État du projet
- Tests : état des suites Unity et de cppcheck
- Travail non commité : liste des fichiers, messages de commit proposés
- Prochaine étape recommandée

### 5. Améliorations de la configuration
Suggestions concrètes pour CLAUDE.md, les skills, les commandes ou
settings.json — uniquement si la session a révélé un manque réel.

## Règles
- Factuel : ne rapporter que ce qui s'est réellement passé dans la session
- Concis : le rapport doit se lire en moins de 5 minutes
