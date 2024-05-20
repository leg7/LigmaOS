#pragma once
#include <architecture/x86/32/isr.h>

u8 PS2_8042_data_read(void);
void PS2_8042_initialize(void);
void PS2_8042_IRQ_12_handler(struct ISR_parameters const *p);

