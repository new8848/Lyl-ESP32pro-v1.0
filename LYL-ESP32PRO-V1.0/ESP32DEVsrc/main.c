#include "mywifi.h"
#include "mymqtt.h"
#include "esp_log.h"
#include "mybeef.h"
#include "mydht22.h"
#include "myled.h"
#include "myfan.h"
#include "mymics.h"
#include "myoled.h"
#include "queuepublic.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "MAIN";
static bool s_env_alarm_active = false;
static bool s_fan_on = false;
static bool s_led_on = false;
static bool s_both_on_latched = false;

static void sync_outputs_and_check_buzzer(void)
{
    myfan_set(s_fan_on);
    gpio_set_level(LED_GPIO_PIN, s_led_on ? 1 : 0);

    if (s_fan_on && s_led_on) {
        if (!s_both_on_latched) {
            s_both_on_latched = true;
            mybeef_trigger();
            ESP_LOGW(TAG, "风扇和灯同时开启，触发蜂鸣器");
        }
    } else {
        s_both_on_latched = false;
    }
}

static void env_alarm_control_update(float temperature, uint32_t concentration)
{
    bool over_limit = (temperature > MYBEEF_TEMP_THRESHOLD) || (concentration > MYBEEF_GAS_THRESHOLD);
    if (over_limit == s_env_alarm_active) {
        return;
    }

    s_env_alarm_active = over_limit;
    s_fan_on = over_limit;
    s_led_on = over_limit;
    sync_outputs_and_check_buzzer();

    if (over_limit) {
        ESP_LOGW(TAG, "环境超阈值，已开启风扇和灯光");
    } else {
        ESP_LOGI(TAG, "环境恢复正常，已关闭风扇和灯光");
    }
}

static void cmd_dispatch_task(void *arg)
{
    char cmd_buf[CMD_DATA_SIZE] = {0};

    while (1) {
        if (xQueueReceive(cmd_queue, cmd_buf, portMAX_DELAY) == pdTRUE) {
            if (strcmp(cmd_buf, "00001") == 0) {
                s_fan_on = true;
                sync_outputs_and_check_buzzer();
                sensor_set_report_command("10001");
                ESP_LOGI(TAG, "FAN ON, report command -> 10001");
            } else if (strcmp(cmd_buf, "01001") == 0) {
                s_fan_on = false;
                sync_outputs_and_check_buzzer();
                sensor_set_report_command("11001");
                ESP_LOGI(TAG, "FAN OFF, report command -> 11001");
            } else if (strcmp(cmd_buf, "00002") == 0) {
                s_led_on = true;
                sync_outputs_and_check_buzzer();
                sensor_set_report_command("10002");
                ESP_LOGI(TAG, "LED ON, report command -> 10002");
            } else if (strcmp(cmd_buf, "01002") == 0) {
                s_led_on = false;
                sync_outputs_and_check_buzzer();
                sensor_set_report_command("11002");
                ESP_LOGI(TAG, "LED OFF, report command -> 11002");
            } else {
                ESP_LOGW(TAG, "Unknown command: %s", cmd_buf);
            }

            memset(cmd_buf, 0, sizeof(cmd_buf));
        }
    }
}

static void mqtt_periodic_report_task(void *arg)
{
    sensor_data_t latest = {0};
    sensor_data_t incoming = {0};
    char report_json[MQTT_REPORT_JSON_BUF_SIZE] = {0};

    while (1) {
        if (g_sensor_data_queue != NULL && xQueuePeek(g_sensor_data_queue, &incoming, 0) == pdTRUE) {
            latest = incoming;
        }

        if (sensor_build_report_json(report_json, sizeof(report_json), &latest) == ESP_OK) {
            int ret = mqtt_publish_data(report_json, (int)strlen(report_json));
            if (ret == 0) {
                ESP_LOGI(TAG, "MQTT周期上报成功: %s", report_json);
            } else {
                ESP_LOGW(TAG, "MQTT周期上报失败(等待连接): %s", report_json);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void network_service_task(void *arg)
{
    if (mywificonnect() == -1) {
        ESP_LOGE(TAG, "WiFi connect failed");
        vTaskDelete(NULL);
        return;
    }

    if (mywifi_sync_time() != ESP_OK) {
        ESP_LOGW(TAG, "time sync failed, OLED will show placeholder time");
    }

    if (mymqttconnect() == -1)
    {
        ESP_LOGE(TAG, "MQTT connect(start) failed");
        vTaskDelete(NULL);
        return;
    }

    vTaskDelete(NULL);
}

static void sensor_collect_task(void *arg)
{
    float t = 0.0f;
    float h = 0.0f;
    sensor_data_t data = {0};

    dht22_init();
    mics5524_init();

    while (1) {
        if (dht22_read(&t, &h) == ESP_OK) {
            data.temperature = t;
            data.humidity = h;
        }

        data.mics_value = mics5524_read_filtered();
        mybeef_check_and_trigger(data.temperature, data.mics_value);
        env_alarm_control_update(data.temperature, data.mics_value);

        if (g_sensor_data_queue != NULL) {
            xQueueOverwrite(g_sensor_data_queue, &data);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void) 
{
    if (myfan_init() != ESP_OK) {
        ESP_LOGE(TAG, "myfan init failed");
        return;
    }

    if (sensor_queue_init() != ESP_OK) {
        ESP_LOGE(TAG, "sensor queue init failed");
        return;
    }

    if (mybeef_init() != ESP_OK) {
        ESP_LOGE(TAG, "mybeef init failed");
        return;
    }

    if (myled_init() != ESP_OK) {
        ESP_LOGE(TAG, "myled init failed");
        return;
    }

    xTaskCreate(sensor_collect_task, "sensor_collect", 4096, NULL, 5, NULL);
    xTaskCreate(cmd_dispatch_task, "cmd_dispatch", 4096, NULL, 5, NULL);
    oled_start_task();
    xTaskCreate(mqtt_periodic_report_task, "mqtt_report_5s", 4096, NULL, 4, NULL);

    if (xTaskCreate(network_service_task, "network_service", 6144, NULL, 4, NULL) != pdPASS) {
        ESP_LOGE(TAG, "create network_service task failed");
        return;
    }
}