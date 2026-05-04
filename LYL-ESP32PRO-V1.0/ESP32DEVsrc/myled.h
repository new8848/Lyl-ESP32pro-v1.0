#ifndef MYLED_H
#define MYLED_H
#include "driver/gpio.h"
#include "esp_err.h"

#define LED_GPIO_PIN  26 // GPIO pin connected to the LED

esp_err_t myled_init(void);
void myled_start_task(void);

#endif