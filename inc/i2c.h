#ifndef I2C_H
#define I2C_H

#include "config.h"

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
bit  i2c_write(unsigned char b);
unsigned char i2c_read(bit ack);

#endif /* I2C_H */