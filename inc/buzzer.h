/**
 * @file buzzer.h
 * @brief Dieu khien buzzer (active-HIGH) qua transistor NPN
 *
 * Buzzer ket noi: P2.7 -> R_base -> NPN -> Buzzer -> VCC
 * Set pin HIGH = buzzer ON, LOW = buzzer OFF
 */
#ifndef BUZZER_H
#define BUZZER_H

void buzzer_on(void);
void buzzer_off(void);

#endif /* BUZZER_H */
