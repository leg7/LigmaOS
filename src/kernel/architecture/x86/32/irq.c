#include "irq.h"
#include "idt.h"
#include "isr.h"
#include <architecture/x86/chips/PIC_8259A.h>
#include <stdio.h>

ISR IRQ_OVERRIDES[16] = { nullptr };

void IRQ_dispatcher(struct ISR_parameters const *p)
{
	u8 const irq = p->interrupt - IDT_REPROGRAMABLE_INTERRUPT_START_INDEX;
	u16 const pending = PIC_8259A_pending();
	u16 const processing = PIC_8259A_processing();

	if (IRQ_OVERRIDES[irq] != nullptr) {
		(IRQ_OVERRIDES[irq])(p);
	} else {
		printf("Unhandeled IRQ %d:\n\tpending irqs: %d\n\tprocessing irqs: %d\n",
		   irq, pending, processing);
	}

	PIC_8259A_eoi(irq);
}

void IRQ_initialize(void)
{
	PIC_8259A_initialize();

	for (u8 i = IDT_REPROGRAMABLE_INTERRUPT_START_INDEX;
	i < IDT_REPROGRAMABLE_INTERRUPT_START_INDEX + 16; ++i) {
		ISR_OVERRIDES[i] = IRQ_dispatcher;
	}

	IRQ_enable();
}

