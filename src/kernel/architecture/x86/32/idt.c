#include "idt.h"
#include "gdt.h"
#include "isr.h"

struct [[gnu::aligned(8)]] [[gnu::packed]] IDT_gate
{
	u16 ISR_address_low;
	u16 kernel_code_segment_selector;
	u8 _zero_1;
	enum IDT_gate_type type : 4;
	u8 _zero_2 : 1;
	u8 ring : 2;
	bool present : 1;
	u16 ISR_address_high;
};

struct IDT_gate IDT[IDT_LENGTH];

struct [[gnu::packed]] IDT_descriptor
{
	u16 size;
	struct IDT_gate *IDT;
};

struct IDT_descriptor IDT_descriptor = {
	.size = sizeof(IDT) - 1,
	.IDT = IDT,
};

void IDT_set_gate(u8 const interrupt, u32 const ISR_address, enum IDT_gate_type const type, u8 ring)
{
	IDT[interrupt].ISR_address_low = ISR_address & 0x00'00'ff'ff;
	IDT[interrupt].ISR_address_high = ISR_address >> 16;

	IDT[interrupt].kernel_code_segment_selector = GDT_SELECTOR_CODE_KERNEL;

	IDT[interrupt].type = type;
	IDT[interrupt].ring = ring;
	IDT[interrupt].present = true;

	IDT[interrupt]._zero_1 = 0;
	IDT[interrupt]._zero_2 = 0;
}

[[gnu::cdecl]] void IDT_load(struct IDT_descriptor d);

void IDT_initialize(void)
{
	u16 interrupt;
	for (interrupt = 0; interrupt < 32; ++interrupt) {
		IDT_set_gate(interrupt, ISRs[interrupt], IDT_GATE_TYPE_TRAP_32_BIT, 0);
	}
	for (; interrupt < IDT_LENGTH; ++interrupt) {
		IDT_set_gate(interrupt, ISRs[interrupt], IDT_GATE_TYPE_INTERRUPT_32_BIT, 0);
	}
	IDT_load(IDT_descriptor);
}
