    #include <stdio.h>
#include "sdkconfig.h"
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "queuepublic.h"
#include "myoled.h"


static const char *TAG = "OLED";
static uint8_t s_oled_addr = OLED_ADDR_PRIMARY;
static i2c_master_bus_handle_t s_i2c_bus = NULL;
static i2c_master_dev_handle_t s_oled_dev = NULL;
static uint8_t s_oled_buffer[OLED_WIDTH * OLED_PAGE_COUNT];

static esp_err_t i2c_master_init(void)
{
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_MASTER_NUM,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    esp_err_t ret = i2c_new_master_bus(&bus_config, &s_i2c_bus);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2c_new_master_bus failed: %s", esp_err_to_name(ret));
        return ret;
    }

    return ESP_OK;
}

static esp_err_t i2c_probe_addr(uint8_t addr)
{
    return i2c_master_probe(s_i2c_bus, addr, 1000);
}

static esp_err_t oled_detect_addr(void)
{
    vTaskDelay(pdMS_TO_TICKS(500));

    for (int round = 0; round < 5; round++) {
        esp_err_t ret = i2c_probe_addr(OLED_ADDR_PRIMARY);
        if (ret == ESP_OK) {
            s_oled_addr = OLED_ADDR_PRIMARY;
            ESP_LOGI(TAG, "OLED address: 0x%02X", s_oled_addr);
            return ESP_OK;
        }
        ESP_LOGW(TAG, "Probe 0x%02X failed: %s", OLED_ADDR_PRIMARY, esp_err_to_name(ret));
        vTaskDelay(pdMS_TO_TICKS(200));
    }

    ESP_LOGE(TAG, "OLED not found at 0x%02X", OLED_ADDR_PRIMARY);
    return ESP_FAIL;
}

static esp_err_t oled_attach_device(void)
{
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = s_oled_addr,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
    };

    esp_err_t ret = i2c_master_bus_add_device(s_i2c_bus, &dev_config, &s_oled_dev);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2c_master_bus_add_device failed: %s", esp_err_to_name(ret));
    }
    return ret;
}

// ==================== OLED 发送 ====================
static esp_err_t oled_send(uint8_t control, const uint8_t *data, size_t len)
{
    uint8_t buffer[17];
    if (len > sizeof(buffer) - 1) {
        ESP_LOGE(TAG, "oled_send len too large: %u", (unsigned)len);
        return ESP_ERR_INVALID_ARG;
    }

    buffer[0] = control;
    if (len > 0) {
        memcpy(&buffer[1], data, len);
    }

    esp_err_t ret = i2c_master_transmit(s_oled_dev, buffer, len + 1, 200);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "OLED send control=0x%02X len=%zu failed: %s", control, len, esp_err_to_name(ret));
    }
    return ret;
}

static inline esp_err_t oled_write_cmd(uint8_t cmd)
{
    return oled_send(0x00, &cmd, 1);
}

static inline esp_err_t oled_write_data_bytes(const uint8_t *data, size_t len)
{
    return oled_send(0x40, data, len);
}

static bool oled_run_init_sequence(const uint8_t *cmds, size_t len, const char *name)
{
    ESP_LOGI(TAG, "Trying init sequence: %s", name);
    for (size_t i = 0; i < len; i++) {
        esp_err_t ret = oled_write_cmd(cmds[i]);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Init sequence %s failed at index %u, cmd=0x%02X, err=%s",
                     name,
                     (unsigned)i,
                     cmds[i],
                     esp_err_to_name(ret));
            return false;
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
    ESP_LOGI(TAG, "Init sequence %s OK", name);
    return true;
}

// ==================== OLED 初始化 ====================
static bool oled_init(void)
{
    const uint8_t init_cmds_internal_vcc[] = {
        0xAE,
        0x20, 0x02,
        0xD5, 0x80,
        0xA8, 0x3F,
        0xD3, 0x00,
        0x40,
        0x8D, 0x14,
        0xA1,
        0xC8,
        0xDA, 0x12,
        0x81, 0x1F,
        0xD9, 0xF1,
        0xDB, 0x30,
        0xA4,
        0xA6,
        0xAF,
    };

    const uint8_t init_cmds_external_vcc[] = {
        0xAE,
        0x20, 0x02,
        0xD5, 0x80,
        0xA8, 0x3F,
        0xD3, 0x00,
        0x40,
        0x8D, 0x10,
        0xA1,
        0xC8,
        0xDA, 0x12,
        0x81, 0x0F,
        0xD9, 0x22,
        0xDB, 0x20,
        0xA4,
        0xA6,
        0xAF,
    };

    vTaskDelay(pdMS_TO_TICKS(300));

    if (oled_run_init_sequence(init_cmds_internal_vcc,
                               sizeof(init_cmds_internal_vcc),
                               "internal_vcc")) {
        return true;
    }

    vTaskDelay(pdMS_TO_TICKS(200));

    if (oled_run_init_sequence(init_cmds_external_vcc,
                               sizeof(init_cmds_external_vcc),
                               "external_vcc")) {
        return true;
    }

    return false;
}

// ==================== 光标 ====================
static esp_err_t oled_set_pos(uint8_t x, uint8_t y)
{
    esp_err_t ret;
    ret = oled_write_cmd(0xB0 + y);
    if (ret != ESP_OK) return ret;
    ret = oled_write_cmd(0x00 | (x & 0x0F));
    if (ret != ESP_OK) return ret;
    return oled_write_cmd(0x10 | (x >> 4));
}

static void oled_clear_buffer(void)
{
    memset(s_oled_buffer, 0, sizeof(s_oled_buffer));
}

static void oled_set_pixel_physical(uint8_t x, uint8_t y, bool on)
{
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT) {
        return;
    }

    size_t index = (y / 8) * OLED_WIDTH + x;
    uint8_t mask = 1U << (y % 8);
    if (on) {
        s_oled_buffer[index] |= mask;
    } else {
        s_oled_buffer[index] &= (uint8_t)~mask;
    }
}

static void oled_set_pixel_rotated(uint8_t x, uint8_t y, bool on)
{
    if (x >= OLED_LOGICAL_WIDTH || y >= OLED_LOGICAL_HEIGHT) {
        return;
    }

    uint8_t physical_x = (uint8_t)(OLED_WIDTH - 1 - (y + OLED_VIEW_X_OFFSET));
    uint8_t physical_y = (uint8_t)(OLED_HEIGHT - 1 - x);
    oled_set_pixel_physical(physical_x, physical_y, on);
}

static esp_err_t oled_flush_buffer(void)
{
    for (uint8_t page = 0; page < OLED_PAGE_COUNT; page++) {
        if (oled_set_pos(0, page) != ESP_OK) {
            ESP_LOGE(TAG, "oled_flush_buffer fail page %d", page);
            return ESP_FAIL;
        }

        for (uint8_t block = 0; block < 8; block++) {
            const uint8_t *data = &s_oled_buffer[page * OLED_WIDTH + block * 16];
            if (oled_write_data_bytes(data, 16) != ESP_OK) {
                ESP_LOGE(TAG, "oled_flush_buffer data fail page %d block %d", page, block);
                return ESP_FAIL;
            }
        }
    }

    return ESP_OK;
}

// ==================== 小字体/标签字模 ====================
static const uint8_t s_font_5x7_digits[10][7] = {
    {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E},
    {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E},
    {0x0E, 0x11, 0x01, 0x06, 0x08, 0x10, 0x1F},
    {0x1E, 0x01, 0x01, 0x0E, 0x01, 0x01, 0x1E},
    {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02},
    {0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E},
    {0x06, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E},
    {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08},
    {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E},
    {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C},
};

static const uint8_t s_font_5x7_dash[7]  = {0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00};
static const uint8_t s_font_5x7_colon[7] = {0x00, 0x04, 0x04, 0x00, 0x04, 0x04, 0x00};
static const uint8_t s_font_5x7_dot[7]   = {0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x06};
static const uint8_t s_font_5x7_percent[7] = {0x19, 0x19, 0x02, 0x04, 0x08, 0x13, 0x13};
static const uint8_t s_font_5x7_degree[7] = {0x06, 0x09, 0x09, 0x06, 0x00, 0x00, 0x00};
static const uint8_t s_font_5x7_T[7] = {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04};
static const uint8_t s_font_5x7_H[7] = {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11};
static const uint8_t s_font_5x7_G[7] = {0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0F};
static const uint8_t s_font_5x7_P[7] = {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10};
static const uint8_t s_font_5x7_M[7] = {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11};
static const uint8_t s_font_5x7_C[7] = {0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E};
static const uint8_t s_font_5x7_S[7] = {0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E};
static const uint8_t s_font_5x7_Y[7] = {0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04};
static const uint8_t s_font_5x7_N[7] = {0x11, 0x19, 0x19, 0x15, 0x13, 0x13, 0x11};
static const uint8_t s_font_5x7_K[7] = {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11};
static const uint8_t s_font_5x7_space[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static const uint8_t s_icon_thermometer[8] = {0x04, 0x0E, 0x0E, 0x0E, 0x0E, 0x1F, 0x1F, 0x0E};
static const uint8_t s_icon_drop[8] = {0x04, 0x0E, 0x1F, 0x1F, 0x1F, 0x1F, 0x0E, 0x04};

static const uint8_t *oled_get_font_5x7(char ch)
{
    if (ch >= '0' && ch <= '9') {
        return s_font_5x7_digits[ch - '0'];
    }

    switch (ch) {
        case '-': return s_font_5x7_dash;
        case ':': return s_font_5x7_colon;
        case '.': return s_font_5x7_dot;
        case '%': return s_font_5x7_percent;
        case '^': return s_font_5x7_degree;
        case 'T': return s_font_5x7_T;
        case 'H': return s_font_5x7_H;
        case 'G': return s_font_5x7_G;
        case 'P': return s_font_5x7_P;
        case 'M': return s_font_5x7_M;
        case 'C': return s_font_5x7_C;
        case 'S': return s_font_5x7_S;
        case 'Y': return s_font_5x7_Y;
        case 'N': return s_font_5x7_N;
        case 'K': return s_font_5x7_K;
        case ' ': return s_font_5x7_space;
        default:  return s_font_5x7_space;
    }
}

static void oled_draw_small_char(uint8_t x, uint8_t y, char ch)
{
    const uint8_t *glyph = oled_get_font_5x7(ch);
    for (uint8_t row = 0; row < 7; row++) {
        for (uint8_t col = 0; col < 5; col++) {
            if (glyph[row] & (1U << (4 - col))) {
                oled_set_pixel_rotated(x + row, y + col, true);
            }
        }
    }
}

static void oled_draw_text(uint8_t x, uint8_t y, const char *text)
{
    for (size_t i = 0; text[i] != '\0'; i++) {
        oled_draw_small_char(x, (uint8_t)(y + i * 6), text[i]);
    }
}

static void oled_draw_text_scaled(uint8_t x, uint8_t y, const char *text, uint8_t scale)
{
    for (size_t idx = 0; text[idx] != '\0'; idx++) {
        const uint8_t *glyph = oled_get_font_5x7(text[idx]);
        uint8_t base_y = (uint8_t)(y + idx * (uint8_t)(6 * scale));

        for (uint8_t row = 0; row < 7; row++) {
            for (uint8_t col = 0; col < 5; col++) {
                if ((glyph[row] & (1U << (4 - col))) == 0) {
                    continue;
                }

                for (uint8_t sx = 0; sx < scale; sx++) {
                    for (uint8_t sy = 0; sy < scale; sy++) {
                        oled_set_pixel_rotated((uint8_t)(x + row * scale + sx),
                                               (uint8_t)(base_y + col * scale + sy),
                                               true);
                    }
                }
            }
        }
    }
}

static uint8_t oled_text_width(const char *text, uint8_t scale)
{
    size_t len = strlen(text);
    if (len == 0) {
        return 0;
    }
    return (uint8_t)(len * 5 * scale + (len - 1) * scale);
}

static void oled_draw_icon8(uint8_t x, uint8_t y, const uint8_t icon[8])
{
    for (uint8_t row = 0; row < 8; row++) {
        for (uint8_t col = 0; col < 5; col++) {
            if (icon[row] & (1U << (4 - col))) {
                oled_set_pixel_rotated((uint8_t)(x + row), (uint8_t)(y + col), true);
            }
        }
    }
}

static bool oled_time_is_valid(void)
{
    return time(NULL) >= 1735689600;
}

static void oled_format_datetime(char *date_buf, size_t date_len, char *time_buf, size_t time_len)
{
    time_t now = time(NULL);
    struct tm tm_info = {0};

    if (oled_time_is_valid()) {
        localtime_r(&now, &tm_info);
        strftime(date_buf, date_len, "%Y-%m-%d", &tm_info);
        strftime(time_buf, time_len, "%H:%M:%S", &tm_info);
    } else {
        snprintf(date_buf, date_len, "SYNC");
        snprintf(time_buf, time_len, "--:--:--");
    }
}

static void oled_draw_sensor_row(uint8_t x,
                                 uint8_t y,
                                 char prefix,
                                 const char *value,
                                 const char *suffix,
                                 const uint8_t *icon)
{
    char line_buf[24] = {0};

    if (suffix != NULL) {
        snprintf(line_buf, sizeof(line_buf), "%c:%s%s", prefix, value, suffix);
    } else {
        snprintf(line_buf, sizeof(line_buf), "%c:%s", prefix, value);
    }

    if (icon != NULL) {
        oled_draw_icon8((uint8_t)(x + 3), y, icon);
    }
    oled_draw_text_scaled(x, (uint8_t)(y + 8), line_buf, 1);
}

static void oled_display_task(void *pvParameters)
{
    if (i2c_master_init() != ESP_OK) {
        ESP_LOGE(TAG, "I2C init failed");
        vTaskDelete(NULL);
        return;
    }

    if (oled_detect_addr() != ESP_OK) {
        vTaskDelete(NULL);
        return;
    }

    if (oled_attach_device() != ESP_OK) {
        vTaskDelete(NULL);
        return;
    }

    if (!oled_init()) {
        ESP_LOGE(TAG, "OLED init failed");
        vTaskDelete(NULL);
        return;
    }

    sensor_data_t latest = {0};
    sensor_data_t incoming = {0};
    char temp_buf[16] = {0};
    char hum_buf[16] = {0};
    char gas_buf[16] = {0};
    char date_buf[16] = {0};
    char time_buf[16] = {0};

    while (1) {
        if (g_sensor_data_queue != NULL &&
            xQueuePeek(g_sensor_data_queue, &incoming, 0) == pdTRUE) {
            latest = incoming;
        }

        snprintf(temp_buf, sizeof(temp_buf), "%.1f", latest.temperature);
        snprintf(hum_buf, sizeof(hum_buf), "%.1f", latest.humidity);
        snprintf(gas_buf, sizeof(gas_buf), "%lu", (unsigned long)latest.mics_value);
        oled_format_datetime(date_buf, sizeof(date_buf), time_buf, sizeof(time_buf));

        oled_clear_buffer();

        {
            oled_draw_sensor_row(2, 2, 'T', temp_buf, "^C", s_icon_thermometer);
            oled_draw_sensor_row(18, 2, 'H', hum_buf, "%", s_icon_drop);
            oled_draw_sensor_row(34, 2, 'G', gas_buf, "PPM", NULL);

            {
                uint8_t date_x = 49;
                uint8_t date_y = 0;
                uint8_t time_x = 56;
                uint8_t time_y = 0;
                uint8_t date_width = oled_text_width(date_buf, 1);
                uint8_t time_width = oled_text_width(time_buf, 1);

                if (date_width < OLED_LOGICAL_HEIGHT) {
                    date_y = (uint8_t)(OLED_LOGICAL_HEIGHT - date_width - 2);
                }
                if (time_width < OLED_LOGICAL_HEIGHT) {
                    time_y = (uint8_t)(OLED_LOGICAL_HEIGHT - time_width - 2);
                }

                oled_draw_text(date_x, date_y, date_buf);
                oled_draw_text(time_x, time_y, time_buf);
            }
        }

        if (oled_flush_buffer() != ESP_OK) {
            ESP_LOGE(TAG, "OLED flush failed");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void oled_start_task(void)
{
    xTaskCreate(oled_display_task, "oled_display", 4096, NULL, 5, NULL);
}
    
