#include "mywifi.h"
#include "esp_mac.h"
#include "esp_sntp.h"
#include <time.h>
static EventGroupHandle_t s_wifi_event_group;
// WiFi 事件回调
static void wifi_event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data) 
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) 
    {
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        (void)event_data;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

// WiFi 初始化（STA模式）
void wifi_init_sta(void)
 {
    s_wifi_event_group = xEventGroupCreate();

    // 1. 初始化 NVS（关键修复）
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());  // 若NVS分区满或版本不匹配，擦除后重新初始化
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);  // 确保NVS初始化成功

    // 2. 初始化网络接口
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    // 3. 初始化 WiFi 驱动
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));  // 现在不会报错了

    // 注册事件回调
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    // 配置热点参数
    wifi_config_t wifi_config = 
    {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());


}

int mywificonnect()
{
    wifi_init_sta();
    // 等待连接
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);
    if (bits & WIFI_CONNECTED_BIT) return 0; // 成功
    else return -1; // 失败
    
}

esp_err_t mywifi_sync_time(void)
{
    const time_t min_valid_time = 1735689600; // 2025-01-01 00:00:00 UTC
    time_t now = time(NULL);

    setenv("TZ", "CST-8", 1);
    tzset();

    if (now >= min_valid_time) {
        return ESP_OK;
    }

    if (!esp_sntp_enabled()) {
        esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
        esp_sntp_setservername(0, "ntp.aliyun.com");
        esp_sntp_setservername(1, "ntp.tencent.com");
        esp_sntp_init();
    }

    for (int i = 0; i < 75; ++i) {
        vTaskDelay(pdMS_TO_TICKS(200));
        now = time(NULL);
        if (now >= min_valid_time) {
            return ESP_OK;
        }
    }

    return ESP_ERR_TIMEOUT;
}                                                                                 