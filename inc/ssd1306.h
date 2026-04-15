/**
 * @file ssd1306.h
 * @brief Driver OLED SSD1306 128x64 qua I2C (text-only, khong framebuffer)
 *
 * Thiet ke cho AT89S52 (256 byte RAM) nen khong dung framebuffer 1KB.
 * Chi ho tro hien thi text voi font 5x7 (6 pixel/ky tu, 21 ky tu/dong, 8 dong).
 */
#ifndef SSD1306_H
#define SSD1306_H

/**
 * Khoi tao OLED SSD1306 128x64
 * Goi sau i2c_init()
 */
void oled_init(void);

/**
 * Xoa toan bo man hinh (ghi 0x00 vao toan bo RAM)
 */
void oled_clear(void);

/**
 * Dat vi tri con tro
 * @param page  Dong (0-7), moi dong cao 8 pixel
 * @param col   Cot pixel (0-127)
 */
void oled_set_cursor(unsigned char page, unsigned char col);

/**
 * In chuoi ky tu tai vi tri con tro hien tai
 * Chi ho tro ASCII 32-90 (space..Z), lowercase tu dong chuyen thanh uppercase
 * @param s  Con tro chuoi ky tu (ket thuc '\0')
 */
void oled_print_str(const char *s);

/**
 * In so nguyen khong dau tai vi tri con tro hien tai
 * @param val  Gia tri 0-65535
 */
void oled_print_uint(unsigned int val);

#endif /* SSD1306_H */
