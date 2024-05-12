#pragma once
#include <types.h>

void PIC_8259A_initialize(void);
void PIC_8259A_eoi(u8 const irq);
s32 PIC_8259A_mask(u8 irq);
s32 PIC_8259A_unmask(u8 irq);
u16 PIC_8259A_irq_requests(void);
u16 PIC_8259A_irq_in_service(void);


