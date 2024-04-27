#include "interrupts.h"

constexpr struct gdt_entry gdt[3] = {
	gdt_entry(0, 0, 0, 0),

	// code segment spanning all of the addressable memory
	gdt_entry(
		0,
		0xff'ff'ff,
		GDT_ENTRY_ACCESS_PRESENT | GDT_ENTRY_ACCESS_RING0 | GDT_ENTRY_ACCESS_CODE | GDT_ENTRY_ACCESS_CODE_READABLE,
		GDT_ENTRY_FLAGS_SEGMENT_WIDTH32_BIT | GDT_ENTRY_FLAGS_GRANULARITY4_K
	),

	// data segment spanning all of the addressable memory
	gdt_entry(
		0,
		0xff'ff'ff,
		GDT_ENTRY_ACCESS_PRESENT | GDT_ENTRY_ACCESS_RING0 | GDT_ENTRY_ACCESS_DATA | GDT_ENTRY_ACCESS_DATA_WRITEABLE | GDT_ENTRY_ACCESS_ACCESSED,
		GDT_ENTRY_FLAGS_SEGMENT_WIDTH32_BIT | GDT_ENTRY_FLAGS_GRANULARITY4_K
	),
};

struct gdt_descriptor gdt_descriptor = {
	.gdt = gdt,
	.size = sizeof(gdt) - 1,
};

void gdt_initialize_x86(void)
{
	// 8 and 16 because a g_d_t entry is 8 bytes
	// so the code entry is located 8 bytes in and the
	// data entry 16 bytes in the table
	gdt_load_x86(&gdt_descriptor, 8, 16);
}

// ----- IDT -----


// #define idt_size 256
// struct idt_gate idt[_idt_size];

// to be used with the `lidt` asm function whose argument is a pointer to an idt_descriptor
// const struct [[gnu::packed]]
// {
// 	const u16 size;
// 	const struct idt_gate *idt;
// } idt_descriptor = {
// 	.size = (sizeof(struct idt_gate) * idt_size) - 1,
// 	.idt = idt
// };
//
// void idt_gate_set(const u8 interrupt, const uintptr_t isr_address, const enum idt_gate_type type, const u8 ring)
// {
// 	idt[interrupt].isr_address_low = isr_address & 0x00'00'_f_f'_f_f;
// 	idt[interrupt].isr_address_high = (isr_address & 0x_f_f'_f_f'00'00) >> 16;
//
// 	idt[interrupt]._gdt_kernel_code_segment = 8;
// 	idt[interrupt].type = type;
// 	idt[interrupt].ring = ring;
// 	idt[interrupt].present = true;
//
// 	idt[interrupt]._zero = 0;
// 	idt[interrupt]._zero2 = 0;
// }

// void idt_initialize_x86(void)
// {
// 	idt_gate_set(0,   (uintptr_t)_isr0,   idt_gate_type32_bit_trap, 0);
// 	idt_load_x86();
// }
