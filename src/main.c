/**
 * @file main.c
 * @brief Chuong trinh chinh - He thong phat hien dong dat
 *
 * Luu do hoat dong:
 *   1. Khoi tao I2C, ADXL345, bo loc, bo phat hien, buzzer
 *   2. Bat ngat ngoai INT0 (nhan tin hieu DATA_READY tu ADXL345)
 *   3. Vong lap chinh:
 *      - Doc gia toc ke -> g_ax, g_ay, g_az
 *      - Loc tin hieu   -> g_mag
 *      - Phat hien dong dat (doc g_mag noi bo)
 *      - Cap nhat buzzer
 *
 * Toi uu RAM:
 *   - Khong co local variable trong main loop:
 *     g_ax/g_ay/g_az/g_mag la shared globals (dinh nghia trong adxl345.c)
 *   - Chuoi goi ham phang: main -> adxl_read_xyz -> i2c_*
 *     khong co wrapper trung gian
 *
 * NOTE: SSD1306 OLED bi vo hieu hoa tam thoi.
 *   AT89S52 chi co 256 byte RAM noi, khong du cho stack + bien toan cuc
 *   cua driver OLED chay dong thoi voi ADXL345 + filter + detect.
 *   Khi can dung lai OLED: bo comment phan [SSD1306] va giam bo nho
 *   cac bo dem tam trong filter/detect truoc.
 *
 * Ket noi phan cung:
 *   ADXL345 INT1 --> P3.2 (INT0) cua AT89S52 (falling edge)
 *   I2C SCL      --> P1.2
 *   I2C SDA      --> P1.3
 *   Buzzer       --> P2.7 (qua NPN transistor)
 */
#include "config.h"
#include "i2c.h"
#include "adxl345.h"
/* #include "ssd1306.h" */   /* SSD1306: vo hieu hoa do AT89S52 het RAM */
#include "filter.h"
#include "detect.h"
#include "buzzer.h"

/* Co bao hieu ADXL345 co data moi (set trong ISR, xoa trong main loop) */
volatile bit data_ready = 0;

/**
 * ISR ngat ngoai 0 (P3.2/INT0)
 * ADXL345 keo INT1 xuong LOW khi co du lieu moi (DATA_READY)
 */
void ext0_isr(void) __interrupt(0)
{
    data_ready = 1;
}

void main(void)
{
    /* === Khoi tao ngoai vi === */
    i2c_init();
    /* oled_init(); */        /* SSD1306: vo hieu hoa do AT89S52 het RAM */
    adxl_init();
    filter_init();
    detect_init();
    buzzer_off();

    /* === Bat ngat ngoai INT0 === */
    EA  = 1;    /* Enable All Interrupts */
    EX0 = 1;    /* Enable External Interrupt 0 */
    IT0 = 1;    /* Falling edge trigger (ADXL345 INT la active-LOW) */

    /* === Vong lap chinh === */
    while (1) {
        if (data_ready) {
            data_ready = 0;

            adxl_read_xyz();    /* -> g_ax, g_ay, g_az */
            filter_update();    /* -> g_mag             */
            detect_update();    /* doc g_mag noi bo     */

            if (detect_is_quake()) {
                buzzer_on();
            } else {
                buzzer_off();
            }

            /*
             * SSD1306 OLED: vo hieu hoa do AT89S52 het RAM
             * oled_set_cursor(3, 48);
             * oled_print_str(detect_is_quake() ? "QUAKE!" : "SAFE  ");
             * oled_set_cursor(5, 30);
             * oled_print_uint(g_mag);
             */
        }
    }
}