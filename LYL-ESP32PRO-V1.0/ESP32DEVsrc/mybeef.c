#include "mybeef.h"

#include <stdbool.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "MYBEEF";
static TaskHandle_t s_beef_task_handle = NULL;
static bool s_beef_busy = false;

static void mybeef_set_level(uint32_t level)
{
	gpio_set_level(MYBEEF_GPIO, level);
}

static void mybeef_alarm_task(void *arg)
{
	while (1) {
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		s_beef_busy = true;
		mybeef_set_level(1);
		ESP_LOGW(TAG, "buzzer on");
		vTaskDelay(pdMS_TO_TICKS(2000));
		mybeef_set_level(0);
		s_beef_busy = false;
		ESP_LOGW(TAG, "buzzer off");
	}
}

esp_err_t mybeef_init(void)
{
	const gpio_config_t io_conf = {
		.pin_bit_mask = 1ULL << MYBEEF_GPIO,
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

	mybeef_set_level(0);

	if (s_beef_task_handle == NULL) {
		BaseType_t ok = xTaskCreate(mybeef_alarm_task, "mybeef_alarm", 2048, NULL, 5, &s_beef_task_handle);
		if (ok != pdPASS) {
			s_beef_task_handle = NULL;
			ESP_LOGE(TAG, "create mybeef_alarm task failed");
			return ESP_FAIL;
		}
	}

	return ESP_OK;
}

void mybeef_trigger(void)
{
	if (s_beef_task_handle == NULL || s_beef_busy) {
		return;
	}

	xTaskNotifyGive(s_beef_task_handle);
}

void mybeef_check_and_trigger(float temperature, uint32_t concentration)
{
	if (temperature > MYBEEF_TEMP_THRESHOLD || concentration > MYBEEF_GAS_THRESHOLD) {
		mybeef_trigger();
	}
}
