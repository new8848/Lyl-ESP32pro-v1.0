#include "queuepublic.h"
#include "esp_log.h"
#include "esp_timer.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#define SENSOR_QUEUE_LEN 1

static const char *TAG = "QUEUE";
static portMUX_TYPE s_report_cmd_lock = portMUX_INITIALIZER_UNLOCKED;
static char s_report_command[CMD_DATA_SIZE] = MQTT_REPORT_FIXED_COMMAND;

QueueHandle_t g_sensor_data_queue = NULL;
QueueHandle_t cmd_queue = NULL;

esp_err_t sensor_queue_init(void)
{
	if (g_sensor_data_queue != NULL && cmd_queue != NULL) {
		return ESP_OK;
	}

	if (g_sensor_data_queue == NULL) {
		g_sensor_data_queue = xQueueCreate(SENSOR_QUEUE_LEN, sizeof(sensor_data_t));
	}
	if (g_sensor_data_queue == NULL) {
		ESP_LOGE(TAG, "create g_sensor_data_queue failed");
		return ESP_FAIL;
	}

	if (cmd_queue == NULL) {
		cmd_queue = xQueueCreate(CMD_QUEUE_LEN, CMD_DATA_SIZE);
	}
	if (cmd_queue == NULL) {
		ESP_LOGE(TAG, "create cmd_queue failed");
		vQueueDelete(g_sensor_data_queue);
		g_sensor_data_queue = NULL;
		return ESP_FAIL;
	}

	return ESP_OK;
}

esp_err_t sensor_build_report_json(char *json_buf, size_t json_buf_len, const sensor_data_t *data)
{
	if (json_buf == NULL || data == NULL || json_buf_len == 0) {
		return ESP_ERR_INVALID_ARG;
	}

	char id_buf[24] = {0};
	char value_buf[64] = {0};
	char command_buf[CMD_DATA_SIZE] = {0};
	int64_t timestamp_ms = esp_timer_get_time() / 1000;

	taskENTER_CRITICAL(&s_report_cmd_lock);
	strncpy(command_buf, s_report_command, sizeof(command_buf) - 1);
	taskEXIT_CRITICAL(&s_report_cmd_lock);

	snprintf(id_buf, sizeof(id_buf), "%" PRId64, (int64_t)timestamp_ms);
	snprintf(value_buf,
		 sizeof(value_buf),
		 MQTT_REPORT_VALUE_FMT,
		 data->temperature,
		 data->humidity,
		 (unsigned long)data->mics_value);

	snprintf(json_buf,
		 json_buf_len,
		 MQTT_REPORT_JSON_TEMPLATE_FMT,
		 id_buf,
		 command_buf,
		 value_buf);

	return ESP_OK;
}

void sensor_set_report_command(const char *cmd)
{
	if (cmd == NULL || cmd[0] == '\0') {
		return;
	}

	taskENTER_CRITICAL(&s_report_cmd_lock);
	memset(s_report_command, 0, sizeof(s_report_command));
	strncpy(s_report_command, cmd, sizeof(s_report_command) - 1);
	taskEXIT_CRITICAL(&s_report_cmd_lock);
}