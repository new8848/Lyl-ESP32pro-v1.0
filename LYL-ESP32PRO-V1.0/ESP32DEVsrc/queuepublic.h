#ifndef QUEUE_PUBLIC_H
#define QUEUE_PUBLIC_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
	float temperature;
	float humidity;
	uint32_t mics_value;
} sensor_data_t;

#define MQTT_REPORT_JSON_TEMPLATE_FMT "{\"id\":\"%s\",\"version\":\"1.0\",\"params\":{\"command\":{\"value\":\"%s\"},\"message\":{\"value\":\"%s\"}}}"
#define MQTT_REPORT_FIXED_COMMAND "00000"
#define MQTT_REPORT_VALUE_FMT "T:%.2f,H:%.2f,G:%lu"
#define MQTT_REPORT_JSON_BUF_SIZE 192

#define CMD_QUEUE_LEN 5
#define CMD_DATA_SIZE 16

extern QueueHandle_t g_sensor_data_queue;
extern QueueHandle_t cmd_queue;

esp_err_t sensor_queue_init(void);
esp_err_t sensor_build_report_json(char *json_buf, size_t json_buf_len, const sensor_data_t *data);
void sensor_set_report_command(const char *cmd);

#endif // QUEUE_PUBLIC_H