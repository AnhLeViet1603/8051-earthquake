/**
 * @file adxl345.c
 * @brief Driver ADXL345 Accelerometer
 *
 * Toi uu RAM:
 *   - adxl_read_xyz(void): ghi truc tiep vao g_ax, g_ay, g_az
 *     thay vi nhan con tro (loai bo 3 pointer param = 6 byte DATA)
 *   - buf[6] khai bao static: linker 8051 co the overlay voi
 *     bien cuc bo cua cac ham khong goi nhau
 *
 * Giao tiep: I2C
 * Dia chi: 0x53 (SDO = GND)
 *
 * Cau hinh:
 *   - Data rate: 25 Hz (ODR = 0x08)
 *   - Range: +-2g, full resolution (13-bit, 3.9 mg/LSB)
 *   - Ngat DATA_READY map len chan INT1 (noi vao P3.2/INT0 cua MCU)
 *
 * Luu y thuc: 1g ~ 256 LSB @ 3.9mg/LSB
 */
#include "config.h"
#include "i2c.h"
#include "adxl345.h"

#define ADXL_ADDR   0x53   /* 7-bit address khi SDO = GND */

/* Dia chi thanh ghi ADXL345 */
#define REG_BW_RATE     0x2C
#define REG_POWER_CTL   0x2D
#define REG_INT_ENABLE  0x2E
#define REG_INT_MAP     0x2F
#define REG_DATA_FORMAT 0x31
#define REG_DATAX0      0x32

/* Dinh nghia cac shared globals (extern trong config.h) */
int          g_ax = 0, g_ay = 0, g_az = 0;
unsigned int g_mag = 0;

static void write_reg(unsigned char reg, unsigned char val)
{
    i2c_start();
    i2c_write((unsigned char)((ADXL_ADDR << 1) | 0));  /* write */
    i2c_write(reg);
    i2c_write(val);
    i2c_stop();
}

void adxl_init(void)
{
    /* Standby mode truoc khi cau hinh */
    write_reg(REG_POWER_CTL, 0x00);

    /* Data format: Full resolution, +-2g */
    write_reg(REG_DATA_FORMAT, 0x08);

    /* Output data rate: 25 Hz */
    write_reg(REG_BW_RATE, 0x08);

    /* Enable INT DATA_READY (bit7) */
    write_reg(REG_INT_ENABLE, 0x80);

    /* Map DATA_READY -> INT1 (bit=0), cac ngat khac -> INT2 */
    write_reg(REG_INT_MAP, 0x00);

    /* Bat do measurement */
    write_reg(REG_POWER_CTL, 0x08);
}

/**
 * Doc gia toc 3 truc, ghi ket qua vao g_ax, g_ay, g_az.
 * buf[6] khai bao static: tranh cap phat tren stack, cho phep
 * linker overlay voi bien cuc bo cua ham khac trong chuong trinh.
 */
void adxl_read_xyz(void)
{
    static unsigned char buf[6];
    unsigned char i;

    /* Write thanh ghi bat dau doc */
    i2c_start();
    i2c_write((unsigned char)((ADXL_ADDR << 1) | 0));  /* write */
    i2c_write(REG_DATAX0);

    /* Repeated start, chuyen sang read mode */
    i2c_start();
    i2c_write((unsigned char)((ADXL_ADDR << 1) | 1));  /* read */

    for (i = 0; i < 6; i++) {
        buf[i] = i2c_read(i < 5);  /* ACK cho 5 byte dau, NACK byte cuoi */
    }
    i2c_stop();

    /* Du lieu dang Little-Endian, 2's complement 13-bit */
    g_ax = (int)((unsigned int)(buf[1] << 8) | buf[0]);
    g_ay = (int)((unsigned int)(buf[3] << 8) | buf[2]);
    g_az = (int)((unsigned int)(buf[5] << 8) | buf[4]);
}