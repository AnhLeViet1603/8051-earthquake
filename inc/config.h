#ifndef CONFIG_H
#define CONFIG_H

#include <reg52.h>

/*
 * Cau hinh pin I2C (Software I2C)
 *   SCL = P1.2  (bit addr 0x92)
 *   SDA = P1.3  (bit addr 0x93)
 *   Tranh xung dot voi T2 (0x90) / T2EX (0x91) trong reg52.h
 */
#define SCL_BIT_ADDR  0x92
#define SDA_BIT_ADDR  0x93

/*
 * Buzzer: P2.7 (active-HIGH)
 * Ket noi qua transistor NPN de drive buzzer
 */
#define BUZZER_BIT_ADDR  0xA7

/* Bien co ngat data-ready tu ADXL345 (set trong ISR) */
extern volatile bit data_ready;

/* NOP macro cho SDCC */
#define _nop_() __asm nop __endasm

/*
 * Shared globals lien module:
 *   g_ax/g_ay/g_az : gia tri gia toc tho tu ADXL345 (ghi boi adxl_read_xyz)
 *   g_mag          : magnitude sau loc (ghi boi filter_update)
 * Muc dich: loai bo truyen tham so qua function params -> giam DATA segment
 */
extern int          g_ax, g_ay, g_az;
extern unsigned int g_mag;

#endif /* CONFIG_H */