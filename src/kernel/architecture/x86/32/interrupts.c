#include "interrupts.h"

constexpr struct GDT_entry GDT[3] = {
	GDT_entry(0, 0, 0, 0),

	// code segment spanning all of the addressable memory
	GDT_entry(
		0,
		0xff'ff'ff,
		GDT_ENTRY_ACCESS_PRESENT | GDT_ENTRY_ACCESS_RING0 | GDT_ENTRY_ACCESS_CODE | GDT_ENTRY_ACCESS_CODE_READABLE,
		GDT_ENTRY_FLAGS_SEGMENT_WIDTH32_BIT | GDT_ENTRY_FLAGS_GRANULARITY4_K
	),

	// data segment spanning all of the addressable memory
	GDT_entry(
		0,
		0xff'ff'ff,
		GDT_ENTRY_ACCESS_PRESENT | GDT_ENTRY_ACCESS_RING0 | GDT_ENTRY_ACCESS_DATA | GDT_ENTRY_ACCESS_DATA_WRITEABLE | GDT_ENTRY_ACCESS_ACCESSED,
		GDT_ENTRY_FLAGS_SEGMENT_WIDTH32_BIT | GDT_ENTRY_FLAGS_GRANULARITY4_K
	),
};

struct GDT_descriptor GDT_descriptor = {
	.GDT = GDT,
	.size = sizeof(GDT) - 1,
};

void GDT_initialize_x86(void)
{
	// 8 and 16 because a g_d_t entry is 8 bytes
	// so the code entry is located 8 bytes in and the
	// data entry 16 bytes in the table
	GDT_load_x86(&GDT_descriptor, 8, 16);
}

// ----- IDT -----


// #define IDT_size 256
// struct IDT_gate IDT[_IDT_size];

// to be used with the `lIDT` asm function whose argument is a pointer to an IDT_descriptor
// const struct [[gnu::packed]]
// {
// 	const u16 size;
// 	const struct IDT_gate *IDT;
// } IDT_descriptor = {
// 	.size = (sizeof(struct IDT_gate) * IDT_size) - 1,
// 	.IDT = IDT
// };
//
// void IDT_gate_set(const u8 interrupt, const uintptr_t ISR_address, const enum IDT_gate_type type, const u8 ring)
// {
// 	IDT[interrupt].ISR_address_low = ISR_address & 0x00'00'_f_f'_f_f;
// 	IDT[interrupt].ISR_address_high = (ISR_address & 0x_f_f'_f_f'00'00) >> 16;
//
// 	IDT[interrupt]._GDT_kernel_code_segment = 8;
// 	IDT[interrupt].type = type;
// 	IDT[interrupt].ring = ring;
// 	IDT[interrupt].present = true;
//
// 	IDT[interrupt]._zero = 0;
// 	IDT[interrupt]._zero2 = 0;
// }

// void IDT_initialize_x86(void)
// {
// 	IDT_gate_set(0,   (uintptr_t)_ISR0,   IDT_gate_type32_bit_trap, 0);
// 	IDT_load_x86();
// }
