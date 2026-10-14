# Contexte projet et journal des décisions

Document de passation : état courant du projet et décisions prises, avec leur
justification. À lire en début de session. À mettre à jour en fin de session
(commande `/report` ou agent `documenter`) dès qu'une décision est prise ou
qu'un point ouvert est réglé.

---

## 1. État courant

| Élément | État |
|---|---|
| Driver GPIO | Implémenté et testé sur carte. Revue `reviewer` faite le 2026-10-13 : corrections à faire (section 6). |
| Driver UART | Esquisse non commitée dans `Drivers/BSP/uart/` (uart.h complet, uart.c vide). |
| Configuration Claude Code | Terminée : CLAUDE.md, settings, hooks, skills, commandes, agents. |
| README | Publié sur `develop`. |

**Branches**
- `main` : branche stable.
- `develop` : intégration. Tout le travail y est poussé.
- `feature/uart-driver` : alignée sur `develop`, aucun commit propre pour l'instant.
- `fix/gpio-review` : à créer depuis `develop` pour les corrections de la revue GPIO.

---

## 2. Outils et build (Windows)

- **make** : utiliser exclusivement le `make.exe` fourni par CubeIDE (chemin
  dans `.claude/commands/build.md`). Le `make` de MSYS2, premier dans le PATH,
  transmet mal `TMP`/`TEMP` à `arm-none-eabi-gcc` et provoque
  *"Cannot create temporary file in C:\WINDOWS\"*. Le chemin contient la
  version du plugin CubeIDE : à mettre à jour après chaque mise à jour de CubeIDE.
- **Makefile** : `Debug/makefile` est généré par CubeIDE. Après ajout d'un
  fichier `.c` ou modification des réglages, relancer d'abord un build CubeIDE.
- **Warnings** : `-Wall -Wextra`, **sans `-Werror`**. Raison : `-Werror` global
  casse sur les warnings de la HAL et de CMSIS, qu'on ne peut pas modifier.
  Objectif : 0 warning dans `Drivers/BSP/`, `Tests/` et `Core/`, vérifié par
  `/build` et cppcheck.
- **Sortie du build** : redirigée vers `build.log` (ignoré par Git) plutôt
  qu'un pipe, qui perdait la sortie.
- **Lancement de Claude Code** : depuis Windows Terminal / PowerShell, pas
  depuis la fenêtre Git Bash (mintty), qui fait passer Claude Code en mode
  non interactif.
- **Taille actuelle** : Flash ≈ 6,8 Ko / 128 Ko, RAM ≈ 1,6 Ko / 32 Ko.

---

## 3. Configuration Claude Code — décisions

- **Mode de permission** : `defaultMode: "default"` (Manual) fixé dans
  `.claude/settings.json`. Auto et Accept edits sont exclus : ils contournent
  la règle « validation requise » de CLAUDE.md.
- **Protections en écriture** : règles `deny` au format `Edit(chemin)`.
  Le format `Write(chemin)` n'est pas appliqué aux fichiers.
- **`allow`** : `Bash(echo*)`, `Bash(cat*)` et `Bash(find*)` retirés.
  Ils permettaient d'écrire ou de supprimer des fichiers par redirection
  ou par `find -delete`, en contournant les règles `Edit` et l'interdiction de `rm`.
  La lecture passe par les outils Read, Grep et Glob.
- **Télémétrie** : coupée dans les réglages utilisateur (`~/.claude/settings.json`),
  pas dans le projet : c'est un choix personnel.
- **Agents** (`.claude/agents/`) :
  - `reviewer` : Opus, lecture seule (outils d'écriture retirés).
  - `tester` : Sonnet, écrit uniquement dans `Tests/` (consigne, non garantie
    techniquement).
  - `documenter` : Sonnet, uniquement les commentaires, le README et `docs/`
    (consigne).
- **Modes d'apprentissage A/B/C/D** : conservés dans CLAUDE.md. Ils ne sont
  pas redondants avec les modes de permission de Claude Code.

---

## 4. Git — conventions

- Conventional Commits, un commit par changement logique.
- La configuration et les corrections transverses se commitent sur `develop` ;
  le travail d'un driver se fait sur sa branche `feature/...`.
- Pour mettre à jour une branche feature après un commit sur `develop` :
  `git merge --ff-only develop` depuis la branche feature.
- Intégration d'une branche dans `develop` : `git merge --no-ff`.
- Les branches mergées sont supprimées (`git branch -d`).
- Unity est un submodule (`vendor/Unity`, v2.7.0) : ne jamais le modifier.
- L'utilisateur fait lui-même les commits : ne jamais supposer l'état du
  dépôt, vérifier avec `git status` et `git log`.

---

## 5. Conventions de code BSP — décisions

Règles complètes : skill `bsp-conventions`.

- **Suffixe `U`, jamais `UL`, et cast `(uint32_t)` sur l'opérande gauche
  des décalages** (décidé le 2026-10-14) : `(uint32_t)0x3U << (pin * 2U)`.
  Masques et comparaisons hors décalage : `U` seul (`& 0xFFU`, `!= 0U`).
  - Pourquoi pas `UL` : sur Cortex-M4, `unsigned int` et `unsigned long`
    font tous deux 32 bits, mais sur les tests host-side, `unsigned long`
    fait 64 bits sous Linux/macOS (LP64) et 32 bits sous Windows (LLP64).
  - Pourquoi le cast : Rule 12.2 (amplitude < largeur du type *essentiel*
    de l'opérande gauche). Un littéral `U` est essentiellement du plus petit
    type qui le contient (`0x3U` → unsigned char, 8 bits), donc
    `0x3U << 30` est non conforme. `(uint32_t)0x3U` est conforme et donne
    un décalage 32 bits sur la cible et sur tous les hôtes.
  - Les macros CMSIS (définies en `UL`) s'utilisent telles quelles :
    U et UL sont tous deux essentiellement non signés (Rule 10.4).
  Remplace l'ancienne convention `UL` des skills `bsp-conventions`,
  `gpio-registers`, `uart-registers` et de l'agent `reviewer`.

---

## 6. Revue de BSP_GPIO_Init() — 2026-10-13

**Constats retenus**
- **B1 (bloquant)** : décalages sur des `int` signés (`0x3 << 30`,
  `0x1 << 31` dans ResetPin, `0xF << 28`) : comportement indéfini.
- **B2 (bloquant)** : la valeur `PUPDR = 0b11` (réservée) est acceptée.
  Retirer `BSP_GPIO_PUPD_RESERVED` de l'enum public.
- **M3** : MODER écrit en premier, avec risque de glitch (push-pull
  transitoire sur une ligne open-drain). Écrire MODER en dernier.
- **M5** : GPIOH n'a que PH0 et PH1 sur le F410RB. Refuser les pins 2 à 15.
- **Règle 15.6** : accolades obligatoires sur tous les `if`.
- **Doc** : pas de Doxygen sur Init ; la table AF de `gpio.h` mentionne des
  périphériques absents du F410 (TIM2, TIM3, OTG_FS) et oublie SPI5.

**Décisions**
- **Règle 15.5** (plusieurs `return`) : dérogation à documenter dans
  `docs/misra_deviations.md` (à créer), avec la règle, la justification,
  la portée et les emplacements.
- **Délai après activation de l'horloge (M2)** : relecture du registre après
  l'écriture. La référence d'errata pour le F410 reste à confirmer.
- **PA13/PA14 (SWD), PC14/PC15 (LSE)** : `@note` dans la doc, pas de refus.
- **Méthode** : l'utilisateur écrit les corrections (mode A), le `reviewer`
  relit le diff, le `tester` ajoute les cas d'erreur (pull `0b11` et PH2
  doivent renvoyer `INVALID`), puis les tests GPIO sont relancés sur la carte
  après chaque commit.

**Plan de commits (branche `fix/gpio-review`)**
```
fix(gpio): use unsigned constants in register shifts
fix(gpio): reject reserved pull configuration
fix(gpio): restrict GPIOH to PH0 and PH1
refactor(gpio): write MODER last in BSP_GPIO_Init
style(gpio): add braces to all if statements
docs(gpio): add Doxygen and fix F410 alternate function table
docs: record MISRA-C:2012 deviations
```

---

## 7. Driver UART — décisions de conception (uart.h)

- `BSP_UART_Handle_t` : instance, copie de la configuration, état global,
  `TxState`/`RxState` indépendants et `volatile` (full-duplex), `ErrorCode`
  en masque de bits (plusieurs erreurs peuvent coexister), contexte des
  transferts par interruption.
- `BSP_UART_STATE_RESET = 0x00` : un handle global ou statique est
  initialisé à zéro par le C, donc démarre à l'état « non initialisé ».
- `BaudRate` en `uint32_t` libre, validé à l'Init contre la PCLK réelle
  (erreur d'arrondi maximale de 2 %).
- La configuration des GPIO (mode AF) est à la charge de l'appelant, avant
  `BSP_UART_Init()`.
- Table des prescalers APB nommée `s_apb_presc_shift` : `APBPrescTable` est
  déjà déclarée par CMSIS (`system_stm32f4xx.h`).
- Effacement de TC : `usart->SR = ~USART_SR_TC` (bit rc_w0), jamais
  `&= ~`, qui est une lecture-modification-écriture concurrente.
- ORE est effacé par la lecture de SR puis de DR.
- Vrai 8E1 : M = 1 (le bit de parité compte dans la longueur du mot).
- USART2 = PA2/PA3 en AF7 (Virtual COM Port du ST-LINK). USART6 utilise AF8.

---

## 8. Points ouverts

- Vérifier que la skill `uart-registers` est alignée avec la section 7
  (nom `s_apb_presc_shift`, méthode d'effacement de TC).
- `main.c` : le bouton B1 (PC13, actif bas, pull-up externe sur la carte)
  est configuré en `PULL_DOWN` + front montant. Vérifier le montage réel.
- Choisir une licence pour le dépôt (README : *To be defined*).
- Tests Unity côté PC (`Tests/host/`) : approche de simulation des registres
  à définir.
- Pixel Agents : éventuellement plus tard, après validation complète de la
  configuration.
