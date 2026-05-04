#ifndef MYFAN_H
#define MYFAN_H
#include "driver/gpio.h"
#include "esp_err.h"
#include <stdbool.h>

#define FAN_GPIO_PIN  32 // GPIO pin connected to the fan

esp_err_t myfan_init(void);
void myfan_set(bool on);

#endif // MYFAN_H