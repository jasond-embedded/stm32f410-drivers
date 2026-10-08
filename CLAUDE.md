# CLAUDE.md — stm32f410-drivers

## Projet
Bibliothèque de drivers embarqués bare-metal pour STM32F410RB (ARM Cortex-M4).
Accès direct aux registres, sans HAL ST. Norme : MISRA-C:2012.
Carte : NUCLEO-F410RB. Compilateur : arm-none-eabi-gcc. Tests : Unity(host-side).

## État actuel
- [x] Driver GPIO — gpio.h / gpio.c / test_gpio.h / test_gpio.c
- [ ] Driver UART — à venir
- [ ] Driver I2C
- [ ] Driver SPI
- [ ] Driver 1-Wire

## Mode d'apprentissage actif : MODE A
Changer avec : "MODE A" | "MODE B" | "MODE C" | "MODE D"
Rappeler le mode actif en début de chaque réponse.

- MODE A : L'agent pose des questions avant toute implémentation
           et attend les réponses de l'utilisateur
- MODE B : L'agent implémente puis explique chaque choix + références RM
           Questions de vérification en fin
- MODE C : L'utilisateur propose une ébauche, l'agent critique et améliore
- MODE D : L'agent choisit le mode selon la nouveauté du sujet

## Interdits absolus
- Utiliser la HAL ST dans le code BSP
- Modifier les fichiers CMSIS ou vendor/
- Supprimer des fichiers
- Contourner la validation des paramètres dans les fonctions publiques
- Accéder à des fichiers hors scope projet
- Exécuter sudo
- Flasher sans confirmation explicite de l'utilisateur

## Autorisations
AUTONOME : lire les fichiers dans le scope projet uniquement

VALIDATION REQUISE — proposer et attendre confirmation explicite :
- Écrire ou modifier du code dans Drivers/BSP/ et Tests/
- Modifier commentaires et documentation
- Lancer la compilation
- Créer de nouveaux fichiers ou modifier l'architecture
- Modifier tout header public déjà validé
- Modifier une fonction déjà testée et validée
- Ajouter une dépendance externe
- Modifier CLAUDE.md ou settings.json
- Toute action Git sans exception
- Toute opération de flash

## Git
- Aucune action Git sans permission explicite
- Conventional Commits obligatoires — feat / fix / test / docs / chore
- Commits atomiques — une préoccupation par commit
- Proposer le message formaté et attendre validation

Règle de choix du type :
- feat  → nouveau code fonctionnel dans Drivers/BSP/
- fix   → correction de bug sur code existant validé
- test  → ajout ou modification dans Tests/
- docs  → Doxygen, README, session reports, CLAUDE.md
- chore → build, configuration, Git, outils, vendor/

## Sauvegardes
Avant toute opération majeure :
créer .backup/YYYY-MM-DD_HH-MM_<description>/ — ignoré par Git

## Rapports de session
Après chaque session ou à la demande de l'utilisateur.
Fichier : docs/session_reports/YYYY-MM-DD.md
Contenu : actions effectuées, propositions, décisions, améliorations possibles,
          suggestion de mise à jour CLAUDE.md si pertinent.

## Commandes disponibles
- `/build`  → compilation croisée ARM + rapport warnings
- `/test`   → tests Unity host-side + cppcheck MISRA  
- `/flash`  → flash STM32_Programmer_CLI (confirmation requise)
- `/report` → rapport de session

## Outils de build (Windows)
- make : utiliser UNIQUEMENT celui de CubeIDE (chemin dans .claude/commands/build.md).
  Le make MSYS2 du PATH casse la compilation (TMP/TEMP mal transmis à gcc).
- Le chemin contient la version du plugin CubeIDE (ex. make.win32_2.2.0.202409170845) :
  il est À METTRE À JOUR après chaque mise à jour de CubeIDE.
  Localiser le nouveau : `find /c/ST -name make.exe`, puis modifier build.md.
- Si make.exe est introuvable au chemin indiqué : s'arrêter et prévenir l'utilisateur,
  ne jamais se rabattre sur un autre make.