#ifndef MYBEEF_H
#define MYBEEF_H

#include <stdint.h>
#include "esp_err.h"

#define MYBEEF_GPIO             25
#define MYBEEF_TEMP_THRESHOLD   60.0f
#define MYBEEF_GAS_THRESHOLD    800U

esp_err_t mybeef_init(void);
void mybeef_trigger(void);
void mybeef_check_and_trigger(float temperature, uint32_t concentration);

#endif
