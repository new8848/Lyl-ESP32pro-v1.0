#ifndef DHT22_H
#define DHT22_H
#include "driver/gpio.h"
#include "esp_err.h"
#include <stdio.h>

#define DHT22_PIN 14// GPIO pin connected to DHT22 data pin
#define DHT22_READ_INTERVAL 5000 // Interval in milliseconds for reading data
void dht22_init();
esp_err_t dht22_read(float *temperature, float *humidity);

#endif
    
