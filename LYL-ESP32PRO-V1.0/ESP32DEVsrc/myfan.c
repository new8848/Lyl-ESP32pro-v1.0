#include "myfan.h"

#include "esp_log.h"

static const char *TAG = "MYFAN";

esp_err_t myfan_init(void)
{
	esp_err_t ret = gpio_reset_pin(FAN_GPIO_PIN);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "gpio_reset_pin failed: %s", esp_err_to_name(ret));
		return ret;
	}

	ret = gpio_set_pull_mode(FAN_GPIO_PIN, GPIO_PULLUP_ONLY);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "gpio_set_pull_mode failed: %s", esp_err_to_name(ret));
		return ret;
	}

	// 高电平停止：先写输出锁存为高，再切换到输出，尽量避免上电瞬间毛刺
	ret = gpio_set_level(FAN_GPIO_PIN, 1);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "gpio_set_level failed: %s", esp_err_to_name(ret));
		return ret;
	}

	ret = gpio_set_direction(FAN_GPIO_PIN, GPIO_MODE_OUTPUT);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "gpio_set_direction failed: %s", esp_err_to_name(ret));
		return ret;
	}

	return ESP_OK;
}

void myfan_set(bool on)
{
	// on=true 低电平转；on=false 高电平停
	gpio_set_level(FAN_GPIO_PIN, on ? 0 : 1);
}

