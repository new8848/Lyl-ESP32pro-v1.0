#include "myled.h"

#include <string.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "queuepublic.h"

static const char *TAG = "MYLED";

static void myled_task(void *arg)
{
	char cmd_buf[CMD_DATA_SIZE] = {0};

	while (1) {
		if (xQueueReceive(cmd_queue, cmd_buf, portMAX_DELAY) == pdTRUE) {
			if (strcmp(cmd_buf, "00002") == 0) {
				gpio_set_level(LED_GPIO_PIN, 1);
				sensor_set_report_command("10002");
				ESP_LOGI(TAG, "LED ON, report command -> 10002");
			} else if (strcmp(cmd_buf, "01002") == 0) {
				gpio_set_level(LED_GPIO_PIN, 0);
				sensor_set_report_command("11002");
				ESP_LOGI(TAG, "LED OFF, report command -> 11002");
			} else {
				ESP_LOGW(TAG, "Unknown command: %s", cmd_buf);
			}

			memset(cmd_buf, 0, sizeof(cmd_buf));
		}
	}
}

esp_err_t myled_init(void)
{
	const gpio_config_t io_conf = {
		.pin_bit_mask = 1ULL << LED_GPIO_PIN,
		.mode = GPIO_MODE_OUTPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE,
	};

	esp_err_t ret = gpio_config(&io_conf);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "gpio_config failed: %s", esp_err_to_name(ret));
		return ret;
	}

	gpio_set_level(LED_GPIO_PIN, 0);
	return ESP_OK;
}

void myled_start_task(void)
{
	if (xTaskCreate(myled_task, "myled_task", 3072, NULL, 5, NULL) != pdPASS) {
		ESP_LOGE(TAG, "create myled_task failed");
	}
}
