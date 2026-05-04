#include "mydht22.h"    
#include "sdkconfig.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"
#include "rom/ets_sys.h"

// ========== 日志/延时配置（无需修改） ==========
static const char *TAG = "DHT22";
static uint8_t dht22_data[5] = {0};
static void  dht22_delay_us(int us)
{
    ets_delay_us(us);
}


// 初始化DHT22引脚（开漏输出+上拉）
void dht22_init(void)
{
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT_OD,    // 单线通信必须开漏输出
        .pin_bit_mask = (1ULL << DHT22_PIN),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE,  // 内部上拉（建议外接10K）
    };
    gpio_config(&io_conf);
    gpio_set_level(DHT22_PIN, 1);  // 空闲状态拉高总线

    // 上电2秒等待传感器稳定（正确）
    vTaskDelay(pdMS_TO_TICKS(2000));
}

// DHT22读取函数（修正所有错误）
esp_err_t dht22_read(float *temperature, float *humidity)
{
    int cnt = 0;

    // ========== 1. 发送起始信号（修正：补全释放总线步骤） ==========
    gpio_set_direction(DHT22_PIN, GPIO_MODE_OUTPUT_OD); // 确保是输出模式
    gpio_set_level(DHT22_PIN, 0);                       // 主机拉低总线
    dht22_delay_us(1000);                               // 拉低≥800us（符合要求）
    gpio_set_level(DHT22_PIN, 1);                       // 释放总线（关键！你之前漏了）
    dht22_delay_us(30);                                 // 等待20-40us，让DHT22响应
    gpio_set_direction(DHT22_PIN, GPIO_MODE_INPUT);      // 切换为输入，等待DHT22响应

    // ========== 2. 等待DHT22响应（修正超时判断逻辑） ==========
    // 等待DHT22拉低总线（响应第一步：80us低电平）
    cnt = 0;
    while (gpio_get_level(DHT22_PIN) == 1 && cnt < 100) {
        dht22_delay_us(1);
        cnt++;
    }
    if (cnt >= 100) {
        ESP_LOGE(TAG, "响应超时：拉低前（DHT22未拉低总线）");
        return ESP_ERR_TIMEOUT;
    }

    // 等待DHT22拉低结束（80us低电平）
    cnt = 0;
    while (gpio_get_level(DHT22_PIN) == 0 && cnt < 100) {
        dht22_delay_us(1);
        cnt++;
    }
    if (cnt >= 100) {
        ESP_LOGE(TAG, "响应超时：拉低中");
        return ESP_ERR_TIMEOUT;
    }

    // 等待DHT22拉高结束（80us高电平，通知主机接收数据）
    cnt = 0;
    while (gpio_get_level(DHT22_PIN) == 1 && cnt < 100) {
        dht22_delay_us(1);
        cnt++;
    }
    if (cnt >= 100) {
        ESP_LOGE(TAG, "响应超时：拉高中");
        return ESP_ERR_TIMEOUT;
    }

    // ========== 3. 读取40位数据（修正：循环内不return，读完所有位） ==========
    memset(dht22_data, 0, sizeof(dht22_data)); // 清空缓存，避免旧数据干扰
    for (int i = 0; i < 40; i++) {
        // 等待数据位起始（DHT22拉低50us）
        cnt = 0;
        while (gpio_get_level(DHT22_PIN) == 0 && cnt < 100) {
            dht22_delay_us(1);
            cnt++;
        }
        if (cnt >= 100) {
            ESP_LOGE(TAG, "数据位超时：拉低");
            return ESP_ERR_TIMEOUT;
        }

        // 测量高电平时长（判断0/1：26-28us=0，70us=1）
        cnt = 0;
        while (gpio_get_level(DHT22_PIN) == 1 && cnt < 100) {
            dht22_delay_us(1);
            cnt++;
        }
        if (cnt >= 100) {
            ESP_LOGE(TAG, "数据位超时：拉高");
            return ESP_ERR_TIMEOUT;
        }

        // 解析数据位（位操作逻辑正确，保留）
        if (cnt > 40) { // 高电平>40us=1，否则=0
            dht22_data[i / 8] |= (1 << (7 - (i % 8)));
        } else {
            dht22_data[i / 8] &= ~(1 << (7 - (i % 8)));
        }
    }

    // ========== 4. 校验和+解析数据（修正：移到循环外，读完40位再解析） ==========
    uint8_t checksum = dht22_data[0] + dht22_data[1] + dht22_data[2] + dht22_data[3];
    if (checksum != dht22_data[4]) {
        ESP_LOGE(TAG, "校验和错误：计算=%d，实际=%d", checksum, dht22_data[4]);
        return ESP_ERR_INVALID_CRC;
    }

    // 解析温湿度（DHT22小数精度）
    *humidity = ((dht22_data[0] << 8) | dht22_data[1]) * 0.1f;
    *temperature = (((dht22_data[2] & 0x7F) << 8) | dht22_data[3]) * 0.1f;
    if (dht22_data[2] & 0x80) { // 处理负温度
        *temperature = -*temperature;
    }

    // ========== 5. 恢复总线（修正：移到最后，读完数据再恢复） ==========
    gpio_set_direction(DHT22_PIN, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(DHT22_PIN, 1);

    ESP_LOGI(TAG, "读取成功：温度=%.1f℃，湿度=%.1f%%RH", *temperature, *humidity);
    return ESP_OK;
}
    
