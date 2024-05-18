#pragma once
#include <architecture/x86/32/isr.h>

void PS2_8042_initialize(void);
void PS2_8042_IRQ_1_handler(struct ISR_parameters const *p);
void PS2_8042_IRQ_12_handler(struct ISR_parameters const *p);

