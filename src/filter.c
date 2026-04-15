/**
 * @file filter.c
 * @brief Bo loc tin hieu dong dat (Earthquake Signal Filter)
 *
 * Thuat toan:
 *   1. Low-pass EMA (alpha=1/4): loai bo nhieu cao tan tu cam bien
 *   2. High-pass filter (~0.875): loai bo thanh phan DC (trong luc)
 *      de chi giu lai phan rung dong
 *
 * Magnitude = |hx| + |hy| + |hz|  (L1 norm, nhanh, khong can sqrt)
 *
 * Toi uu RAM:
 *   - filter_update(void): doc g_ax/g_ay/g_az tu shared global,
 *     ghi g_mag khi xong (gop voi filter_get_magnitude cu)
 *     => loai bo 3 int param (6 byte) va 1 function call rieng le
 *   - Ket qua g_mag duoc detect_update() doc truc tiep
 */
#include "config.h"
#include "filter.h"

static int fx, fy, fz;          /* EMA low-pass output */
static int hx, hy, hz;          /* High-pass output    */
static int prev_fx, prev_fy, prev_fz;  /* Gia tri EMA ky truoc */

#define ABS(v) ((v) < 0 ? -(v) : (v))

void filter_init(void)
{
    fx = fy = fz = 0;
    hx = hy = hz = 0;
    prev_fx = prev_fy = prev_fz = 0;
}

/**
 * Cap nhat bo loc voi gia tri gia toc moi tu shared globals (g_ax, g_ay, g_az).
 * Ghi ket qua magnitude vao g_mag khi xong.
 * Thay the cho cap doi filter_update() + filter_get_magnitude() truoc day.
 */
void filter_update(void)
{
    /* EMA low-pass: alpha = 1/4 (dich phai 2 bit) */
    fx += (g_ax - fx) >> 2;
    fy += (g_ay - fy) >> 2;
    fz += (g_az - fz) >> 2;

    /* High-pass: H[n] = 0.875*H[n-1] + (LP[n] - LP[n-1])
     * 0.875 ~ (1 - 1/8) => H = H - (H>>3) + delta */
    hx = hx - (hx >> 3) + (fx - prev_fx);
    hy = hy - (hy >> 3) + (fy - prev_fy);
    hz = hz - (hz >> 3) + (fz - prev_fz);

    prev_fx = fx;
    prev_fy = fy;
    prev_fz = fz;

    /* Tinh magnitude va ghi thang vao shared global */
    g_mag = (unsigned int)ABS(hx)
          + (unsigned int)ABS(hy)
          + (unsigned int)ABS(hz);
}