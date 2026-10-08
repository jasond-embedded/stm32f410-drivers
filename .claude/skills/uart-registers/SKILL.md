---
description: Registres USART du STM32F410RB — SR, DR, BRR, CR1, CR2, CR3, calcul baud rate, horloges APB, pins AF, patterns polling et interruption. Charger pour toute implémentation ou debug du driver UART BSP.
paths:
  - "Drivers/BSP/uart/**"
  - "Tests/src/test_uart*"
  - "Tests/inc/test_uart*"
---

# Registres UART — STM32F410RB (RM0401 Rev4)

## Adresses de base

```
USART1_BASE = 0x40011000  (APB2 — fPCLK2, max 100 MHz)
USART2_BASE = 0x40004400  (APB1 — fPCLK1, max 50 MHz)
USART6_BASE = 0x40011400  (APB2 — fPCLK2, max 100 MHz)

Débit max (datasheet Table 7) : USART1/USART6 12.5 Mbit/s, USART2 6.25 Mbit/s
```

## RCC — Activation et reset des horloges USART

### RCC_APB1ENR — USART2 (RM0401 Section 5.3.9, offset 0x40)

```
bit 17  USART2EN  → horloge USART2
```

### RCC_APB2ENR — USART1, USART6 (RM0401 Section 5.3.10, offset 0x44)

```
bit 4  USART1EN  → horloge USART1
bit 5  USART6EN  → horloge USART6
```

```c
RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
/* NE PAS utiliser RCC_APBxLPENR — registres Low Power uniquement */
```

### RCC_APB1RSTR / RCC_APB2RSTR — reset périphérique (Sections 5.3.6 / 5.3.7)

```
APB1RSTR bit 17  USART2RST
APB2RSTR bit 4   USART1RST
APB2RSTR bit 5   USART6RST
Écrire 1 puis 0 → remet tous les registres USART à leur valeur de reset
Utilisé pour BSP_UART_DeInit()
```

```c
RCC->APB1RSTR |=  RCC_APB1RSTR_USART2RST;
RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;
```

## Horloge du périphérique — fPCLK (RM0401 Section 5.3.3, RCC_CFGR)

```
RCC_CFGR bits 15:13  PPRE2[2:0]  prescaler APB2 (USART1, USART6)
RCC_CFGR bits 12:10  PPRE1[2:0]  prescaler APB1 (USART2)

0xx → /1   100 → /2   101 → /4   110 → /8   111 → /16
```

```c
/* Nom distinct de APBPrescTable : CMSIS déclare déjà un
 * extern const uint8_t APBPrescTable[8] dans system_stm32f4xx.h.
 * Un static du même nom provoque une erreur de compilation. */
static const uint8_t s_apb_presc_shift[8] = {0U, 0U, 0U, 0U, 1U, 2U, 3U, 4U};

static uint32_t get_pclk_hz(const USART_TypeDef *instance)
{
    uint32_t hclk = SystemCoreClock;
    uint32_t ppre;

    if (instance == USART2) {
        ppre = (RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos;
    } else {
        ppre = (RCC->CFGR & RCC_CFGR_PPRE2) >> RCC_CFGR_PPRE2_Pos;
    }
    return hclk >> s_apb_presc_shift[ppre];
}
/* SystemCoreClock n'est exact qu'après SystemCoreClockUpdate()
 * (appelé par la HAL après SystemClock_Config) */
```

## Calcul du baud rate (RM0401 Section 24.4.4)

```
baud = fCK / (8 × (2 - OVER8) × USARTDIV)
OVER8=0 (x16, utilisé dans ce projet) : USARTDIV = fCK / (16 × baud)

BRR = [DIV_Mantissa(12 bits) | DIV_Fraction(4 bits)] = USARTDIV × 16
    → avec OVER8=0 : BRR = fCK / baud   (arrondi au plus proche)
Si l'arrondi de la fraction déborde (=16), la retenue passe dans la
mantisse — la division entière arrondie gère ce cas automatiquement.
OVER8=1 : formule différente (fraction sur 3 bits, bit 3 à 0) — non supporté.
```

```c
uint32_t brr = (pclk_hz + (baud_rate / 2U)) / baud_rate;

/* Validation : mantisse >= 1 et BRR sur 16 bits */
if ((brr < 16U) || (brr > 0xFFFFU)) {
    return BSP_UART_INVALID;
}
usart->BRR = brr;
```

### Tolérance du récepteur (RM0401 Section 24.4.5, Tables 120/121)

```
Déviation totale (émetteur + quantification + récepteur + ligne) < tolérance
OVER8=0, ONEBIT=0 :  M=0 → 3.75% (fraction=0) / 3.33% (fraction≠0)
                     M=1 → 3.41% (fraction=0) / 3.03% (fraction≠0)
L'erreur de quantification BRR n'est qu'une partie du budget :
une marge de conception de 2% sur BRR est raisonnable.
Erreur BRR (%) = |pclk/brr - baud| × 100 / baud
```

## Registres USART — offset depuis USARTx_BASE

```
offset 0x00  SR    Status register
offset 0x04  DR    Data register (TDR en écriture, RDR en lecture)
offset 0x08  BRR   Baud rate register
offset 0x0C  CR1   Control register 1
offset 0x10  CR2   Control register 2
offset 0x14  CR3   Control register 3
offset 0x18  GTPR  Guard time and prescaler (Smartcard uniquement)
Accès en mots 32 bits uniquement
```

## SR — Status Register (RM0401 Section 24.6.1)

```
reset value : 0x00C0 0000 → TXE=1 et TC=1 dès le reset
Activer TCIE immédiatement → IRQ immédiate (TC déjà à 1)

bit 9  CTS   rc_w0  changement nCTS
bit 8  LBD   rc_w0  LIN break détecté
bit 7  TXE   r      TDR vide — clear par écriture DR
bit 6  TC    rc_w0  trame terminée ET TXE=1 — clear : lecture SR puis écriture DR
bit 5  RXNE  rc_w0  donnée reçue dans RDR — clear par lecture DR
bit 4  IDLE  r      ligne idle — clear : lecture SR puis lecture DR
bit 3  ORE   r      overrun — clear : lecture SR puis lecture DR
bit 2  NF    r      bruit — clear : lecture SR puis lecture DR
bit 1  FE    r      framing error / break — clear : lecture SR puis lecture DR
bit 0  PE    r      erreur de parité — attendre RXNE, puis lecture SR puis accès DR
```

```c
/* Clear TC seul — écriture directe, JAMAIS SR &= ~flag :
 * le read-modify-write peut effacer un RXNE arrivé entre lecture et écriture.
 * Écrire 1 sur un bit rc_w0 n'a aucun effet → seul TC est effacé. */
usart->SR = ~USART_SR_TC;

/* Clear des flags d'erreur (ORE, NF, FE, PE, IDLE) — séquence obligatoire */
uint32_t sr = usart->SR;               /* 1. lecture SR (capture les flags) */
(void)usart->DR;                       /* 2. lecture DR */
if ((sr & USART_SR_ORE) != 0UL) { /* ... */ }
```

## DR — Data Register (RM0401 Section 24.6.2)

```
bits 8:0  DR[8:0]  — TDR en écriture, RDR en lecture
Écriture → clear TXE   Lecture → clear RXNE
Avec parité : le MSB (bit 7 si M=0, bit 8 si M=1) est remplacé par le
bit de parité en TX ; en RX le MSB lu est le bit de parité reçu
```

```c
usart->DR = (uint32_t)data & 0xFFUL;               /* 8 bits */
uint8_t rx = (uint8_t)(usart->DR & 0xFFUL);        /* 8 bits */
/* 9 bits (M=1, PCE=0) : masque 0x1FFUL */
```

## BRR — Baud Rate Register (RM0401 Section 24.6.3)

```
bits 15:4  DIV_Mantissa[11:0]
bits  3:0  DIV_Fraction[3:0]
Ne pas modifier pendant une communication.
Les compteurs de baud s'arrêtent si TE / RE sont désactivés.
```

## CR1 — Control Register 1 (RM0401 Section 24.6.4)

```
bit 15  OVER8   0=x16  1=x8   (ce projet : 0)
bit 13  UE      USART enable
bit 12  M       0 = 8 bits données  1 = 9 bits — ne pas modifier pendant un transfert
bit 11  WAKE    méthode de réveil (mute mode)
bit 10  PCE     parity control enable
bit 9   PS      0=paire  1=impaire
bit 8   PEIE    IRQ sur PE
bit 7   TXEIE   IRQ sur TXE
bit 6   TCIE    IRQ sur TC
bit 5   RXNEIE  IRQ sur RXNE ET sur ORE
bit 4   IDLEIE  IRQ sur IDLE
bit 3   TE      transmitter enable — une trame idle est envoyée à l'activation
bit 2   RE      receiver enable — ne pas désactiver pendant une réception
bit 0   SBK     send break
```

### Format de trame selon M et PCE (RM0401 Table 122)

```
M=0 PCE=0 → | SB | 8 bits data      | STB |   8N1
M=0 PCE=1 → | SB | 7 bits data | PB | STB |   7E1 / 7O1
M=1 PCE=0 → | SB | 9 bits data      | STB |   9N1
M=1 PCE=1 → | SB | 8 bits data | PB | STB |   8E1 / 8O1  ← M=1 obligatoire
```

## CR2 — Control Register 2 (RM0401 Section 24.6.5)

```
bits 13:12  STOP[1:0]
            00 = 1 stop   01 = 0.5 stop   10 = 2 stop   11 = 1.5 stop
0.5 et 1.5 : réservés au mode Smartcard
Bits LINEN, CLKEN (synchrone) : laisser à 0 en mode asynchrone
```

```c
usart->CR2 &= ~USART_CR2_STOP;                                  /* clear */
usart->CR2 |=  ((uint32_t)config->stop_bits << USART_CR2_STOP_Pos);
```

## CR3 — Control Register 3 (RM0401 Section 24.6.6)

```
bit 11  ONEBIT  1 échantillon au lieu de 3 — désactive NF
bit 10  CTSIE   IRQ sur CTS
bit 9   CTSE    CTS enable (flow control)
bit 8   RTSE    RTS enable (flow control)
bit 7   DMAT    DMA TX
bit 6   DMAR    DMA RX
bit 5   SCEN    Smartcard mode
bit 4   NACK    Smartcard NACK
bit 3   HDSEL   half-duplex single wire
bit 2   IRLP    IrDA low-power
bit 1   IREN    IrDA enable
bit 0   EIE     IRQ erreurs (NF/ORE/FE) — multibuffer DMA uniquement
Mode asynchrone full-duplex standard : CR3 = 0
```

## Séquence d'initialisation (RM0401 Sections 24.4.2 / 24.4.3)

```
Ordre du RM : UE=1 → M → STOP → BRR → TE/RE
Ordre retenu (pratique courante, équivalent) :
1. Activer l'horloge RCC de l'USART
2. (GPIO TX/RX en AF configurés par l'appelant — voir section Pins)
3. CR1 : UE=0 — configuration hors transfert (M ne doit pas changer en cours)
4. CR1 : M, PCE, PS, OVER8=0
5. CR2 : STOP
6. CR3 : 0
7. BRR : calcul depuis fPCLK
8. CR1 : TE et/ou RE
9. CR1 : UE=1 — en dernier
Après TE=1 : délai d'1 bit, puis envoi d'une trame idle
```

## Pattern TX polling

```c
for (i = 0U; i < size; i++) {
    /* attendre TDR vide — avec timeout (HAL_GetTick ou compteur) */
    while ((usart->SR & USART_SR_TXE) == 0UL) { /* timeout → BSP_UART_TIMEOUT */ }
    usart->DR = (uint32_t)pData[i];
}
/* attendre la fin physique du dernier octet avant de rendre la main */
while ((usart->SR & USART_SR_TC) == 0UL) { /* timeout */ }
```

## Pattern RX polling

```c
while ((usart->SR & USART_SR_RXNE) == 0UL) { /* timeout → BSP_UART_TIMEOUT */ }
sr = usart->SR;                            /* capturer les erreurs AVANT DR */
pData[i] = (uint8_t)(usart->DR & 0xFFUL);  /* clear RXNE (+ flags erreur) */
if ((sr & (USART_SR_ORE | USART_SR_NF | USART_SR_FE | USART_SR_PE)) != 0UL) { /* ErrorCode */ }
```

## Pattern TX par interruption (TXEIE + TCIE)

```c
/* Démarrage : activer TXEIE — l'ISR écrit le premier octet (TXE=1 au repos) */
usart->CR1 |= USART_CR1_TXEIE;

/* ISR — TXE */
if (((sr & USART_SR_TXE) != 0UL) && ((cr1 & USART_CR1_TXEIE) != 0UL)) {
    if (tx_count > 0U) {
        usart->DR = (uint32_t)*p_tx++;
        tx_count--;
    } else {
        usart->CR1 &= ~USART_CR1_TXEIE;   /* plus de données */
        usart->CR1 |=  USART_CR1_TCIE;    /* attendre fin physique */
    }
}
/* ISR — TC */
if (((sr & USART_SR_TC) != 0UL) && ((cr1 & USART_CR1_TCIE) != 0UL)) {
    usart->CR1 &= ~USART_CR1_TCIE;
    /* TxState = IDLE → BSP_UART_TxCpltCallback() */
}
```

## Pattern RX par interruption (RXNEIE)

```c
/* Démarrage */
usart->CR1 |= USART_CR1_RXNEIE | USART_CR1_PEIE;

/* ISR — RXNE ou ORE (RXNEIE déclenche aussi sur ORE) */
if (((sr & (USART_SR_RXNE | USART_SR_ORE)) != 0UL) && ((cr1 & USART_CR1_RXNEIE) != 0UL)) {
    uint8_t byte = (uint8_t)(usart->DR & 0xFFUL);  /* lecture DR : clear RXNE ET ORE */
    /* ORE non clearé → l'IRQ se redéclenche en boucle */
    if (rx_count > 0U) { *p_rx++ = byte; rx_count--; }
    if (rx_count == 0U) {
        usart->CR1 &= ~(USART_CR1_RXNEIE | USART_CR1_PEIE);
        /* RxState = IDLE → BSP_UART_RxCpltCallback() */
    }
}
```

## Structure de l'IRQ handler

```
Un seul vecteur par USART pour tous les événements (RM0401 Section 24.5).
1. Lire SR et CR1 une seule fois au début
2. Traiter les erreurs (PE, FE, NF, ORE) → ErrorCode, ErrorCallback
3. Traiter RXNE si RXNEIE actif
4. Traiter TXE si TXEIE actif
5. Traiter TC si TCIE actif
Toujours tester le flag ET son bit d'enable : TXE et TC sont à 1 au repos.
```

## NVIC — IRQn USART (table des vecteurs startup_stm32f410rx.s)

```
USART1_IRQn = 37   → ISER[1] bit 5
USART2_IRQn = 38   → ISER[1] bit 6
USART6_IRQn = 71   → ISER[2] bit 7
4 bits de priorité → 0 à 15
```

```c
NVIC_SetPriority(USART2_IRQn, priority);
NVIC_EnableIRQ(USART2_IRQn);
/* Handler : void USART2_IRQHandler(void) → BSP_UART_IRQHandler(&huart2); */
```

## Pins UART — STM32F410RB (Datasheet Table 10)

### USART1 (APB2) — AF7

```
PA9   USART1_TX   (alternatifs : PB6, PA15)
PA10  USART1_RX   (alternatifs : PB7, PB3)
PA8   USART1_CK
PA11  USART1_CTS
PA12  USART1_RTS
```

### USART2 (APB1) — AF7 — Virtual COM Port ST-LINK sur Nucleo

```
PA2   USART2_TX   ← relié au ST-LINK (port COM virtuel USB)
PA3   USART2_RX   ← relié au ST-LINK
PA4   USART2_CK
PA0   USART2_CTS
PA1   USART2_RTS
```

### USART6 (APB2) — AF8 (attention : pas AF7)

```
PC6   USART6_TX   (alternatif : PA11)
PC7   USART6_RX   (alternatif : PA12)
PC8   USART6_CK
```

### Configuration GPIO requise (par l'appelant, via BSP_GPIO)

```
TX : BSP_GPIO_MODE_AF, PUSH_PULL, SPEED_HIGH ou VERY_HIGH, PUPD_NONE
RX : BSP_GPIO_MODE_AF, PUPD_PULL_UP recommandé (ligne au repos = HIGH)
AF7 pour USART1/USART2, AF8 pour USART6
USART2 PA2/PA3 : premier choix pour les tests (terminal série via USB)
```

## Référence documentation

```
RM0401 Rev4 — Chapitre 24 USART (24.4.2 TX, 24.4.3 RX, 24.4.4 baud,
              24.4.5 tolérance, 24.4.7 parité, 24.5 IRQ, 24.6 registres)
RM0401 Rev4 — Section 5.3.3 RCC_CFGR, 5.3.6/5.3.7 RSTR, 5.3.9/5.3.10 ENR
Datasheet STM32F410RB (DS11144) — Table 7 (débits), Table 10 (AF mapping)
PM0214 — NVIC
```

Si information manquante dans ce skill → demander la section RM0401 concernée.
