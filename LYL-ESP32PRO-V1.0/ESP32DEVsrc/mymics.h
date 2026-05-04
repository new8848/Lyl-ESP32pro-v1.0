#ifndef MYMICS_H
#define MYMICS_H

#include <stdint.h>

void mics5524_init(void);
uint32_t mics5524_read_raw(void);
uint32_t mics5524_read_filtered(void);

#endif
