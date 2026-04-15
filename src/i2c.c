#include "config.h"
#include "i2c.h"

/*
 * I2C Software Bit-Bang
 * SCL = P1.2 (0x92) -- tranh xung dot voi T2   (0x90)
 * SDA = P1.3 (0x93) -- tranh xung dot voi T2EX (0x91)
 *
 * Ket noi:
 *   P1.2 --> SCL cua ADXL345 va SSD1306
 *   P1.3 --> SDA cua ADXL345 va SSD1306
 *   Them dien tro pull-up 4.7k len VCC (3.3V)
 */
__sbit __at (0x92) SCL;  /* P1.2 */
__sbit __at (0x93) SDA;  /* P1.3 */

/* Delay khoang 5us @ 12MHz */
#define I2C_DELAY() { _nop_(); _nop_(); _nop_(); _nop_(); }

void i2c_init(void)
{
    SDA = 1;
    SCL = 1;
}

void i2c_start(void)
{
    SDA = 1; SCL = 1; I2C_DELAY();
    SDA = 0; I2C_DELAY();
    SCL = 0;
}

void i2c_stop(void)
{
    SDA = 0; SCL = 1; I2C_DELAY();
    SDA = 1; I2C_DELAY();
}

/* Tra ve 1 neu co ACK tu slave */
bit i2c_write(unsigned char b)
{
    unsigned char i;
    bit ack;

    for (i = 0; i < 8; i++) {
        SDA = (b & 0x80) ? 1 : 0;
        SCL = 1; I2C_DELAY();
        SCL = 0; I2C_DELAY();
        b <<= 1;
    }

    /* Doc ACK */
    SDA = 1;
    SCL = 1; I2C_DELAY();
    ack = !SDA;
    SCL = 0;
    return ack;
}

/* ack=1: gui ACK (tiep tuc doc), ack=0: gui NACK (doc cuoi) */
unsigned char i2c_read(bit ack)
{
    unsigned char i;
    unsigned char b = 0;

    SDA = 1;
    for (i = 0; i < 8; i++) {
        SCL = 1; I2C_DELAY();
        b = (b << 1) | (SDA ? 1 : 0);
        SCL = 0; I2C_DELAY();
    }

    SDA = (ack ? 0 : 1);
    SCL = 1; I2C_DELAY();
    SCL = 0;
    SDA = 1;
    return b;
}