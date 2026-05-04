#include "mymqtt.h"
#include "esp_log.h"
#include "cJSON.h"
#include "queuepublic.h"


#define MAX_ID_LEN      16  // id最大长度
static char recv_id[MAX_ID_LEN] = {0};     // 存储id
#define MAX_CMD_LEN     16  // command最大长度
static char g_mqtt_command[MAX_CMD_LEN] = {0};
#define REPLY_JSON_TEMPLATE "{\"id\":\"%s\",\"code\":200,\"msg\":\"success\"}"
#define REPLY_BUF_SIZE 64

// 全局客户端句柄
static esp_mqtt_client_handle_t s_mqtt_client = NULL;

// MQTT 回调函数
static void mqtt_event_handler_cb(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    esp_mqtt_client_handle_t client = event->client;
    switch (event->event_id)
     {
        case MQTT_EVENT_CONNECTED:
            esp_mqtt_client_subscribe(client, SUB_TOPIC, 0);
            break;
        case MQTT_EVENT_DISCONNECTED:
           // vTaskDelay(pdMS_TO_TICKS(3000));
            esp_mqtt_client_reconnect(client);
            break;
        case MQTT_EVENT_DATA:               
            //收到的假如是$sys/83bcAx554p/Linux_my_client_start/thing/property/set主题消息
            if (strncmp(event->topic, SUB_TOPIC, event->topic_len) == 0)
            {
                char* data = strndup(event->data, event->data_len);
                if (data == NULL) break;

                cJSON* root = cJSON_Parse(data);
                if (root == NULL) 
                {
                    free(data);
                    break;
                }

                // 提取id+发布set_reply 
                cJSON* id_node = cJSON_GetObjectItem(root, "id");
                if (id_node && cJSON_IsString(id_node)) 
                {
                    memset(recv_id, 0, sizeof(recv_id));
                    strncpy(recv_id, id_node->valuestring, sizeof(recv_id)-1);
                }
                char reply_buf[REPLY_BUF_SIZE] = {0};
                snprintf(reply_buf, REPLY_BUF_SIZE, REPLY_JSON_TEMPLATE, recv_id);
                esp_mqtt_client_publish
                (
                    client, 
                    PUB_TOPIC, 
                    reply_buf, 
                    strlen(reply_buf), 
                    0,  // QoS
                    0   // 保留位
                );

                // 在set_reply之后，只解析 params.command 并入全局指令队列
                // 期望整体格式:
                //                {
                //    "id": "11",
                //    "version": "1.0",
                //     "params": {
                //         "command": "00002"
               //     }
               //     }
                cJSON* params_node = cJSON_GetObjectItem(root, "params");
                if (params_node && cJSON_IsObject(params_node)) {
                    cJSON* cmd_node = cJSON_GetObjectItem(params_node, "command");
                    if (cmd_node && cJSON_IsString(cmd_node)) {
                        memset(g_mqtt_command, 0, sizeof(g_mqtt_command));
                        strncpy(g_mqtt_command, cmd_node->valuestring, sizeof(g_mqtt_command) - 1);

                        if (cmd_queue != NULL && strlen(g_mqtt_command) > 0) {
                            BaseType_t qret = xQueueSend(cmd_queue, g_mqtt_command, 0);
                            if (qret == pdTRUE) {
                                ESP_LOGI(MQTT_TAG, "command入队成功: %s", g_mqtt_command);
                            } else {
                                ESP_LOGW(MQTT_TAG, "command入队失败(队列满): %s", g_mqtt_command);
                            }
                        } else {
                            ESP_LOGW(MQTT_TAG, "cmd_queue未就绪或command为空");
                        }
                    }
                }
            
                cJSON_Delete(root);
                free(data);
            }
            break;
            default:
            break;
    }
}



// MQTT 连接函数
int mymqttconnect()
{
    // OneNET MQTT 配置
    static const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER,
        .credentials.client_id = ONENET_DN,
        .credentials.username = ONENET_PID,
        .credentials.authentication.password = ONENET_TOKEN,
    };

    // 1. 初始化客户端（赋值给全局句柄）
    s_mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    if (s_mqtt_client == NULL) {
        ESP_LOGE(MQTT_TAG, "客户端初始化失败");
        return -1;
    }

    // 2. 先注册回调
    esp_mqtt_client_register_event(
        s_mqtt_client, 
        ESP_EVENT_ANY_ID, 
        mqtt_event_handler_cb, 
        s_mqtt_client
    );

    // 3. 启动客户端
    esp_err_t start_err = esp_mqtt_client_start(s_mqtt_client);
    if (start_err != ESP_OK) {
        ESP_LOGE(MQTT_TAG, "客户端启动失败，错误码：%d", start_err);
        esp_mqtt_client_destroy(s_mqtt_client);
        s_mqtt_client = NULL;
        return -1;
    }

    ESP_LOGI(MQTT_TAG, "MQTT客户端启动成功");
    return 0;
}

//mqtt上报接口函数
int mqtt_publish_data(const char* data, int len)
{
    if (s_mqtt_client == NULL) {
        ESP_LOGE(MQTT_TAG, "MQTT客户端未初始化");
        return -1;
    }

    int msg_id = esp_mqtt_client_publish(
        s_mqtt_client, 
        PUB_TOPIC1, 
        data, 
        len, 
        0,  // QoS
        0   // 保留位
    );

    if (msg_id == -1) {
        ESP_LOGE(MQTT_TAG, "消息发布失败");
        return -1;
    }

    ESP_LOGI(MQTT_TAG, "消息发布成功，消息ID：%d", msg_id);
    return 0;
}
