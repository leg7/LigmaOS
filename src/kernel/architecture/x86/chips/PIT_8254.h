#pragma once
#include <architecture/x86/32/isr.h>

void PIT_8254_initialize(void);
void PIT_8254_IRQ_0_handler(struct ISR_parameters const *p);
void PIT_8254_frequency_play(u16 const frequency);
void PIT_8254_frequency_stop(void);
