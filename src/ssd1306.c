/**
 * @file ssd1306.c
 * @brief Driver OLED SSD1306 128x64 I2C (text-only, khong framebuffer)
 *
 * Giao tiep: I2C (dung chung bus voi ADXL345)
 * Dia chi: 0x3C (7-bit, SA0 = GND - dia chi mac dinh)
 *
 * Font: 5x7 pixel, luu trong code memory (__code)
 * Moi ky tu chiem 6 pixel ngang (5 font + 1 spacing)
 * => 21 ky tu / dong, 8 dong (page)
 *
 * Khong dung framebuffer vi AT89S52 chi co 256 byte RAM.
 * Ghi truc tiep len OLED qua I2C, dung burst mode de tang toc.
 */
#include "config.h"
#include "i2c.h"
#include "ssd1306.h"

#define OLED_ADDR   0x3C    /* 7-bit I2C address */

/* I2C control bytes cho SSD1306 */
#define CTRL_CMD    0x00    /* Co=0, D/C#=0 : gui lenh */
#define CTRL_DATA   0x40    /* Co=0, D/C#=1 : gui data */

/* ------------------------------------------------------------------ */
/*  Ham noi bo                                                        */
/* ------------------------------------------------------------------ */

/* Gui 1 byte lenh den SSD1306 */
static void oled_cmd(unsigned char cmd)
{
    i2c_start();
    i2c_write((OLED_ADDR << 1) | 0);
    i2c_write(CTRL_CMD);
    i2c_write(cmd);
    i2c_stop();
}

/* Delay don gian ~ms (xap xi, 12MHz crystal, 12 clk/cycle) */
static void delay_ms(unsigned int ms)
{
    unsigned int i;
    while (ms--) {
        for (i = 0; i < 200; i++) {
            _nop_();
        }
    }
}

/* ------------------------------------------------------------------ */
/*  Font 5x7 - ASCII 32..90 (space .. Z)                              */
/*  Luu trong code memory (__code) de khong chiem RAM                 */
/*  Moi ky tu 5 byte, column-major, bit0 = top pixel                 */
/*  Tong: 59 ky tu x 5 byte = 295 byte code space                    */
/* ------------------------------------------------------------------ */
static __code unsigned char font5x7[] = {
    /* 32 ' ' */ 0x00, 0x00, 0x00, 0x00, 0x00,
    /* 33 '!' */ 0x00, 0x00, 0x5F, 0x00, 0x00,
    /* 34 '"' */ 0x00, 0x07, 0x00, 0x07, 0x00,
    /* 35 '#' */ 0x14, 0x7F, 0x14, 0x7F, 0x14,
    /* 36 '$' */ 0x24, 0x2A, 0x7F, 0x2A, 0x12,
    /* 37 '%' */ 0x23, 0x13, 0x08, 0x64, 0x62,
    /* 38 '&' */ 0x36, 0x49, 0x55, 0x22, 0x50,
    /* 39 ''' */ 0x00, 0x05, 0x03, 0x00, 0x00,
    /* 40 '(' */ 0x00, 0x1C, 0x22, 0x41, 0x00,
    /* 41 ')' */ 0x00, 0x41, 0x22, 0x1C, 0x00,
    /* 42 '*' */ 0x14, 0x08, 0x3E, 0x08, 0x14,
    /* 43 '+' */ 0x08, 0x08, 0x3E, 0x08, 0x08,
    /* 44 ',' */ 0x00, 0x50, 0x30, 0x00, 0x00,
    /* 45 '-' */ 0x08, 0x08, 0x08, 0x08, 0x08,
    /* 46 '.' */ 0x00, 0x60, 0x60, 0x00, 0x00,
    /* 47 '/' */ 0x20, 0x10, 0x08, 0x04, 0x02,
    /* 48 '0' */ 0x3E, 0x51, 0x49, 0x45, 0x3E,
    /* 49 '1' */ 0x00, 0x42, 0x7F, 0x40, 0x00,
    /* 50 '2' */ 0x42, 0x61, 0x51, 0x49, 0x46,
    /* 51 '3' */ 0x21, 0x41, 0x45, 0x4B, 0x31,
    /* 52 '4' */ 0x18, 0x14, 0x12, 0x7F, 0x10,
    /* 53 '5' */ 0x27, 0x45, 0x45, 0x45, 0x39,
    /* 54 '6' */ 0x3C, 0x4A, 0x49, 0x49, 0x30,
    /* 55 '7' */ 0x01, 0x71, 0x09, 0x05, 0x03,
    /* 56 '8' */ 0x36, 0x49, 0x49, 0x49, 0x36,
    /* 57 '9' */ 0x06, 0x49, 0x49, 0x29, 0x1E,
    /* 58 ':' */ 0x00, 0x36, 0x36, 0x00, 0x00,
    /* 59 ';' */ 0x00, 0x56, 0x36, 0x00, 0x00,
    /* 60 '<' */ 0x08, 0x14, 0x22, 0x41, 0x00,
    /* 61 '=' */ 0x14, 0x14, 0x14, 0x14, 0x14,
    /* 62 '>' */ 0x00, 0x41, 0x22, 0x14, 0x08,
    /* 63 '?' */ 0x02, 0x01, 0x51, 0x09, 0x06,
    /* 64 '@' */ 0x32, 0x49, 0x79, 0x41, 0x3E,
    /* 65 'A' */ 0x7E, 0x11, 0x11, 0x11, 0x7E,
    /* 66 'B' */ 0x7F, 0x49, 0x49, 0x49, 0x36,
    /* 67 'C' */ 0x3E, 0x41, 0x41, 0x41, 0x22,
    /* 68 'D' */ 0x7F, 0x41, 0x41, 0x22, 0x1C,
    /* 69 'E' */ 0x7F, 0x49, 0x49, 0x49, 0x41,
    /* 70 'F' */ 0x7F, 0x09, 0x09, 0x09, 0x01,
    /* 71 'G' */ 0x3E, 0x41, 0x49, 0x49, 0x7A,
    /* 72 'H' */ 0x7F, 0x08, 0x08, 0x08, 0x7F,
    /* 73 'I' */ 0x00, 0x41, 0x7F, 0x41, 0x00,
    /* 74 'J' */ 0x20, 0x40, 0x41, 0x3F, 0x01,
    /* 75 'K' */ 0x7F, 0x08, 0x14, 0x22, 0x41,
    /* 76 'L' */ 0x7F, 0x40, 0x40, 0x40, 0x40,
    /* 77 'M' */ 0x7F, 0x02, 0x0C, 0x02, 0x7F,
    /* 78 'N' */ 0x7F, 0x04, 0x08, 0x10, 0x7F,
    /* 79 'O' */ 0x3E, 0x41, 0x41, 0x41, 0x3E,
    /* 80 'P' */ 0x7F, 0x09, 0x09, 0x09, 0x06,
    /* 81 'Q' */ 0x3E, 0x41, 0x51, 0x21, 0x5E,
    /* 82 'R' */ 0x7F, 0x09, 0x19, 0x29, 0x46,
    /* 83 'S' */ 0x46, 0x49, 0x49, 0x49, 0x31,
    /* 84 'T' */ 0x01, 0x01, 0x7F, 0x01, 0x01,
    /* 85 'U' */ 0x3F, 0x40, 0x40, 0x40, 0x3F,
    /* 86 'V' */ 0x1F, 0x20, 0x40, 0x20, 0x1F,
    /* 87 'W' */ 0x3F, 0x40, 0x38, 0x40, 0x3F,
    /* 88 'X' */ 0x63, 0x14, 0x08, 0x14, 0x63,
    /* 89 'Y' */ 0x07, 0x08, 0x70, 0x08, 0x07,
    /* 90 'Z' */ 0x61, 0x51, 0x49, 0x45, 0x43,
};

/**
 * In 1 ky tu tai vi tri con tro hien tai
 * Dung I2C burst mode (gui 6 byte data trong 1 transaction)
 * de tang toc gap ~6 lan so voi gui tung byte.
 */
static void oled_putchar(char c)
{
    unsigned char i;
    unsigned int idx;

    /* Chuyen lowercase -> uppercase */
    if (c >= 'a' && c <= 'z') c -= 32;

    /* Kiem tra gioi han font */
    if (c < 32 || c > 90) c = 32;  /* Ky tu khong ho tro -> space */

    idx = (unsigned int)(c - 32) * 5;

    /* Burst mode: gui 5 byte font + 1 byte spacing trong 1 I2C transaction */
    i2c_start();
    i2c_write((OLED_ADDR << 1) | 0);
    i2c_write(CTRL_DATA);
    for (i = 0; i < 5; i++) {
        i2c_write(font5x7[idx + i]);
    }
    i2c_write(0x00);  /* 1 pixel spacing giua cac ky tu */
    i2c_stop();
}

/* ------------------------------------------------------------------ */
/*  Ham public                                                        */
/* ------------------------------------------------------------------ */

void oled_init(void)
{
    delay_ms(100);  /* Cho SSD1306 on dinh sau khi cap nguon */

    /* === Chuoi khoi tao SSD1306 128x64 === */
    oled_cmd(0xAE);         /* Display OFF */

    oled_cmd(0xD5);         /* Set display clock divide ratio */
    oled_cmd(0x80);         /* Suggested ratio 0x80 */

    oled_cmd(0xA8);         /* Set multiplex ratio */
    oled_cmd(0x3F);         /* 1/64 duty (64 lines) */

    oled_cmd(0xD3);         /* Set display offset */
    oled_cmd(0x00);         /* No offset */

    oled_cmd(0x40);         /* Set start line = 0 */

    oled_cmd(0x8D);         /* Charge pump setting */
    oled_cmd(0x14);         /* Enable charge pump (can cho module khong co VCC rieng) */

    oled_cmd(0x20);         /* Memory addressing mode */
    oled_cmd(0x02);         /* Page addressing mode */

    oled_cmd(0xA1);         /* Segment re-map: col 127 = SEG0 */
    oled_cmd(0xC8);         /* COM output scan: remapped (lat man hinh) */

    oled_cmd(0xDA);         /* Set COM pins hardware configuration */
    oled_cmd(0x12);         /* Alternative COM pin, disable remap */

    oled_cmd(0x81);         /* Set contrast control */
    oled_cmd(0xCF);         /* Contrast = 0xCF (kha sang) */

    oled_cmd(0xD9);         /* Set pre-charge period */
    oled_cmd(0xF1);         /* Phase1=1, Phase2=15 */

    oled_cmd(0xDB);         /* Set VCOMH deselect level */
    oled_cmd(0x40);         /* ~0.77 x VCC */

    oled_cmd(0xA4);         /* Entire display ON - output follows RAM */
    oled_cmd(0xA6);         /* Normal display (khong dao mau) */

    oled_cmd(0xAF);         /* Display ON */

    delay_ms(10);           /* Cho display on dinh */
}

void oled_clear(void)
{
    unsigned char page, col;

    for (page = 0; page < 8; page++) {
        /* Dat cursor ve dau dong */
        oled_cmd(0xB0 | page);  /* Set page */
        oled_cmd(0x00);         /* Lower column = 0 */
        oled_cmd(0x10);         /* Upper column = 0 */

        /* Burst mode: ghi 128 byte zero trong 1 I2C transaction */
        i2c_start();
        i2c_write((OLED_ADDR << 1) | 0);
        i2c_write(CTRL_DATA);
        for (col = 0; col < 128; col++) {
            i2c_write(0x00);
        }
        i2c_stop();
    }
}

void oled_set_cursor(unsigned char page, unsigned char col)
{
    oled_cmd(0xB0 | (page & 0x07));             /* Set page address */
    oled_cmd(0x00 | (col & 0x0F));              /* Set lower column nibble */
    oled_cmd(0x10 | ((col >> 4) & 0x0F));       /* Set upper column nibble */
}

void oled_print_str(const char *s)
{
    while (*s) {
        oled_putchar(*s++);
    }
}

void oled_print_uint(unsigned int val)
{
    unsigned char buf[6];
    unsigned char i = 0;
    unsigned char j;

    if (val == 0) {
        oled_putchar('0');
        return;
    }

    /* Tach tung chu so vao buffer (thu tu nguoc) */
    while (val > 0 && i < 5) {
        buf[i++] = '0' + (unsigned char)(val % 10);
        val /= 10;
    }

    /* In ra theo thu tu dung */
    for (j = i; j > 0; j--) {
        oled_putchar(buf[j - 1]);
    }
}
