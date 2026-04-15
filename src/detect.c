/**
 * @file detect.c
 * @brief Bo phat hien dong dat (Earthquake Detector)
 *
 * Thuat toan debounce:
 *   - magnitude > THRESH_HIGH: tang counter (tich luy rung)
 *   - magnitude < THRESH_LOW:  giam counter (on dinh)
 *   - counter >= DEBOUNCE_CNT: xac nhan co dong dat (quake=1)
 *   - counter == 0:            tro ve binh thuong (quake=0)
 *
 * Nguong (scale ADXL345 3.9mg/LSB, Full-res +-2g):
 *   THRESH_HIGH = 80  ~ 312 mg (rung kha manh)
 *   THRESH_LOW  = 25  ~  98 mg (vung an toan)
 *   DEBOUNCE_CNT= 5   ~ 200ms @ 25Hz (5 sample lien tiep)
 *
 * Bien ALERT_HOLD giu trang thai QUAKE it nhat N sample
 * sau khi tat de tranh nhay lien tuc.
 *
 * Toi uu RAM:
 *   - detect_update(void): doc g_mag tu shared global
 *     => loai bo 1 unsigned int param (2 byte)
 */
#include "config.h"
#include "detect.h"

#define THRESH_HIGH   80
#define THRESH_LOW    25
#define DEBOUNCE_CNT   5
#define ALERT_HOLD    50   /* ~2s @ 25Hz */

static unsigned char counter;
static unsigned char hold_cnt;
static unsigned char quake_flag;

void detect_init(void)
{
    counter    = 0;
    hold_cnt   = 0;
    quake_flag = 0;
}

/**
 * Cap nhat trang thai phat hien dong dat.
 * Doc g_mag tu shared global (ghi boi filter_update).
 */
void detect_update(void)
{
    if (g_mag > THRESH_HIGH) {
        if (counter < DEBOUNCE_CNT) counter++;
    } else if (g_mag < THRESH_LOW) {
        if (counter > 0) counter--;
    }

    if (counter >= DEBOUNCE_CNT) {
        quake_flag = 1;
        hold_cnt   = ALERT_HOLD;
    } else if (hold_cnt > 0) {
        hold_cnt--;
        /* quake_flag van = 1, cho het hold */
    } else {
        quake_flag = 0;
    }
}

unsigned char detect_is_quake(void)
{
    return quake_flag;
}