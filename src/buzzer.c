/**
 * @file buzzer.c
 * @brief Dieu khien buzzer active-HIGH qua transistor NPN
 *
 * Ket noi phan cung:
 *   P2.7 (0xA7) --> R_base (1K) --> Base NPN (2N2222/BC547)
 *   Collector --> Buzzer (-) --> Buzzer (+) --> VCC (5V)
 *   Emitter   --> GND
 *
 * Active buzzer: chi can set pin HIGH la keu, LOW la tat.
 */
#include "config.h"
#include "buzzer.h"

/* Buzzer output pin: P2.7 */
__sbit __at (BUZZER_BIT_ADDR) BUZZER;

void buzzer_on(void)
{
    BUZZER = 1;
}

void buzzer_off(void)
{
    BUZZER = 0;
}
