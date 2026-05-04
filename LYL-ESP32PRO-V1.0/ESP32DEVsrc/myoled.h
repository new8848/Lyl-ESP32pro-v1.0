#ifndef MYOLED_H
#define MYOLED_H
#include "driver/gpio.h"
#define I2C_MASTER_SCL_IO   22
#define I2C_MASTER_SDA_IO   21
#define I2C_MASTER_NUM      I2C_NUM_0
#define I2C_MASTER_FREQ_HZ  10000
#define OLED_ADDR_PRIMARY   0x3C
#define OLED_WIDTH          128
#define OLED_HEIGHT         64
#define OLED_PAGE_COUNT     (OLED_HEIGHT / 8)
#define OLED_LOGICAL_WIDTH  64
#define OLED_LOGICAL_HEIGHT 128
#define OLED_VIEW_X_OFFSET  0


void oled_start_task(void);

#endif // MYOLED_H