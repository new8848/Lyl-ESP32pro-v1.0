#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_log.h"
#include "mymics.h"

// 配置：AO = GPIO36 = ADC1_CHANNEL_0
#define MICS_AO_PIN       ADC1_CHANNEL_0
static const char *TAG = "MiCS5524";

// 初始化 ADC（按文档要求：上电预热1秒）
void mics5524_init(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(MICS_AO_PIN, ADC_ATTEN_DB_11);

    ESP_LOGI(TAG, "上电预热 1 秒...");
    vTaskDelay(pdMS_TO_TICKS(1000));
    ESP_LOGI(TAG, "预热完成");
}

// 读原始值
uint32_t mics5524_read_raw(void)
{
    return adc1_get_raw(MICS_AO_PIN);
}

// 10次滤波
uint32_t mics5524_read_filtered(void)
{
    uint32_t sum = 0;
    for (int i = 0; i < 10; i++) {
        sum += adc1_get_raw(MICS_AO_PIN);
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    return sum / 10;
}

