#include "irq.h"
#include "idt.h"
#include "isr.h"
#include <architecture/x86/chips/PIC_8259A.h>
#include <architecture/x86/chips/PS2_8042.h>
#include <drivers/input/PS2_keyboard.h>
#include <drivers/time/RTC.h>
#include <architecture/x86/io.h>
#include <stdio.h>

static ISR IRQ_OVERRIDES[16] = { nullptr };

static const char* IRQ_MESSAGES[16] = {
	"System timer",
	"Keyboard controller",
	"Serial port COM 2",
	"Serial port COM 1",
	"Line print terminal 2",
	"Floppy controller",
	"Line print terminal 1",
	"RTC timer",
	"Mouse controller",
	"Math co-processor",
	"ATA channel 1",
	"ATA channel 2",
};

// TODO: Get rid of indirection
static void dispatcher(struct ISR_parameters const *p)
{
	u8 const irq = p->interrupt - IDT_REPROGRAMABLE_INTERRUPT_START_INDEX;

	if (IRQ_OVERRIDES[irq] != nullptr) {
		IRQ_OVERRIDES[irq](p);
		PIC_8259A_eoi(irq);
		return;
	}

	u16 const pending = PIC_8259A_pending();
	x86_io_wait();
	u16 const processing = PIC_8259A_processing();
	printf("Unhandeled IRQ %d: %s\n\tpending irqs: %d\n\tprocessing irqs: %d\n",
	  irq, IRQ_MESSAGES[irq], pending, processing);

	PIC_8259A_eoi(irq);
}

void IRQ_initialize(void)
{
	PIC_8259A_initialize();

	for (u8 i = IDT_REPROGRAMABLE_INTERRUPT_START_INDEX;
	i < IDT_REPROGRAMABLE_INTERRUPT_START_INDEX + 16; ++i) {
		ISR_OVERRIDES[i] = dispatcher;
	}

	IRQ_OVERRIDES[1] = &PS2_keyboard_IRQ_1_handler;
	IRQ_OVERRIDES[8] = &RTC_IRQ_8_handler;
	IRQ_OVERRIDES[12] = &PS2_8042_IRQ_12_handler;

	IRQ_enable();
}
