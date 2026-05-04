#ifndef MYWIFI_H
#define MYWIFI_H
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_err.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"  // 包含 NVS 头文件

// 适配你的热点参数
#define WIFI_SSID      "OPPOFindX8F94C"
#define WIFI_PASS      "88888888"
#define WIFI_CONNECTED_BIT BIT0

// 把 TAG 改为数组形式的 const，确保字符串放到 flash
int mywificonnect();
esp_err_t mywifi_sync_time(void);
#endif

 