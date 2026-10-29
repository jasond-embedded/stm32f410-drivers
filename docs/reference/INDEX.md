# Documentation de référence — index

Documents ST du projet. Seul ce fichier est versionné : les PDF et leurs
versions texte sont ignorés par Git (droits d'auteur, taille). Après un
clone, télécharger les documents sur st.com et les placer comme indiqué.

## Méthode de recherche

1. Chercher avec Grep dans `txt/` (nom de registre, de bit, de section).
2. Lire seulement les lignes utiles autour du résultat.
3. Ouvrir le PDF uniquement aux pages trouvées, pour les tableaux et les
   figures (l'extraction texte les abîme).
4. Toute affirmation cite : document, révision, section (ou table).

Numéros de page : ceux du sommaire du document, identiques aux numéros de
page du PDF (pied de page « n/total »).

## Documents

| Document | Révision | Date | PDF | Texte |
|---|---|---|---|---|
| RM0401 — STM32F410 reference manual | Rev 4 | février 2025 | `pdf/RM0401_rev4.pdf` | `txt/RM0401_rev4.txt` |
| DS11144 — STM32F410x8/xB datasheet | Rev 7 | janvier 2025 | `pdf/DS11144_rev7.pdf` | `txt/DS11144_rev7.txt` |
| PM0214 — Cortex-M4 programming manual | Rev 10 | mars 2020 | `pdf/PM0214_rev10.pdf` | `txt/PM0214_rev10.txt` |
| UM1724 — STM32 Nucleo-64 boards user manual | Rev 17 | septembre 2025 | `pdf/UM1724_rev17.pdf` | `txt/UM1724_rev17.txt` |
| ES0325 — STM32F410x8/xB device errata | Rev 7 | février 2025 | `pdf/ES0325_rev7.pdf` | `txt/ES0325_rev7.txt` |

Chaque fichier `txt/` commence par une ligne `Source:` (document, révision,
PDF d'origine, date d'extraction avec `pdftotext -layout`).

Extraction : RM0401 et PM0214 signalent des erreurs de flux compressé
(probablement des images) ; toutes les pages ont du texte, mais des
figures peuvent manquer → ouvrir le PDF pour les figures.

## RM0401 Rev 4 — chapitres

| Ch. | Sujet | Page |
|---|---|---|
| 2 | System and memory overview (carte mémoire, bus) | — |
| 3 | Embedded flash memory interface | — |
| 4 | Power controller (PWR) | 71 |
| 5 | Reset and clock control (RCC) | 94 |
| 6 | General-purpose I/Os (GPIO) | 138 |
| 7 | System configuration controller (SYSCFG) | 158 |
| 8 | Direct memory access controller (DMA) | 165 |
| 9 | Interrupts and events (vecteurs, EXTI) | 200 |
| 10 | CRC calculation unit | 213 |
| 11 | Analog-to-digital converter (ADC) | 217 |
| 12 | Digital-to-analog converter (DAC) | 245 |
| 13 | True random number generator (RNG) | 259 |
| 14 | Advanced-control timers (TIM1) | 270 |
| 15 | General-purpose timers (TIM5) | 340 |
| 16 | General-purpose timers (TIM9 and TIM11) | 390 |
| 17 | Basic timers (TIM6) | 437 |
| 18 | Low-power timer (LPTIM) | 450 |
| 19 | Window watchdog (WWDG) | 472 |
| 20 | Independent watchdog (IWDG) | 479 |
| 21 | Real-time clock (RTC) | 485 |
| 22 | Fast-mode Plus I2C (FMPI2C) | 523 |
| 23 | Inter-integrated circuit (I2C) interface | 586 |
| 24 | Universal synchronous receiver transmitter (USART) | — |
| 25 | SPI / I2S | 670 |
| 26 | Debug support (DBG) | 722 |
| 27 | Device electronic signature | 753 |

Les pages marquées « — » n'ont pas pu être lues dans le sommaire extrait :
chercher le titre du chapitre avec Grep.

## DS11144 Rev 7 — sections et tables utiles

| Référence | Sujet | Page |
|---|---|---|
| §3 | Functional overview (dont §3.27 USART, §3.31 GPIOs) | 15 |
| §4 | Pinouts and pin description | 31 |
| Table 9 | STM32F410x8/B pin definitions (broches par boîtier, dont LQFP64) | 34 |
| Table 10 | Alternate function mapping (AF0 à AF15 par broche) | 40 |
| §5 | Memory mapping | 43 |
| §6 | Electrical characteristics | 47 |

Tables 9 et 10 : mise en page abîmée dans le texte → les lire dans le PDF.

## PM0214 Rev 10 — sections utiles

| Référence | Sujet | Page |
|---|---|---|
| ch. 2 | The Cortex-M4 processor (modes, exceptions) | — |
| ch. 4 | Core peripherals | 193 |
| §4.3 | Nested vectored interrupt controller (NVIC) | — |
| §4.4 | System control block (SCB) | — |
| §4.5 | SysTick timer (STK) | — |
| §4.6 | Floating point unit (FPU) | — |

## UM1724 Rev 17 — sujets utiles

La numérotation du sommaire extrait est incohérente avec le corps du
document : chercher par mot-clé plutôt que par numéro de section.

| Sujet | Mot-clé Grep |
|---|---|
| LED utilisateur LD2 | `LD2` |
| Bouton utilisateur B1 | `Push-buttons`, `B1` |
| Horloge (HSE, MCO du ST-LINK) | `OSC clock` |
| Port COM virtuel (USART2 ↔ ST-LINK) | `USART communication`, `Virtual COM` |
| Ponts de soudure | `Solder bridges`, `SB` |
| Connecteurs Arduino / Morpho | `Arduino`, `Morpho` |

## ES0325 Rev 7 — errata utiles

Dans le texte extrait, la numérotation des sections est désordonnée
(tableau récapitulatif et corps ne concordent pas) : chercher par titre,
et lire le numéro de section dans le PDF.

| Sujet | Titre à chercher | Page |
|---|---|---|
| Délai après activation d'une horloge RCC (M2) | `Delay after an RCC peripheral clock enabling` | 9 |
| PC13 perturbe le LSE | `PC13 signal transitions disturb LSE` | 10 |
