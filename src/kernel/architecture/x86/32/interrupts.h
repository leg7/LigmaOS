#pragma once
#include <types.h>

[[gnu::cdecl]] void IRQ_enable(void);
[[gnu::cdecl]] void IRQ_disable(void);

/* --------------------------- GDT ---------------------------
 * 32bit GDT table implementation for x86_32
 * although we won't be using segmentation, but virtual memory and paging,
 * setting this up is necessary to handle interrupts because the IDT will use the GDT
 * to look up ISRs.
 * we will configure it so that segments span the whole 4GiB of addressable memory */

// to be used with the `lGDT` function in asm whose arument is a pointer to a GDT descriptor
struct [[gnu::packed]] GDT_descriptor
{
	const u16 size; 		// sizeof(GDT) - 1
	const struct GDT_entry* GDT;
};

struct [[gnu::packed]] GDT_entry
{
	u16 limit_low;
	u16 base_low;
	u8 base_middle;
	u8 access; 		// defined with bitwise o_r operations on the members of the anonymous enum below
	u8 limit_high : 4;
	u8 flags : 4;	// defined like access
	u8 base_high;
};

// base u32, limit u20, use enum for the rest
#define GDT_entry(base, limit, _access, _flags) \
{ \
	.base_low                 = base & 0x00'00'ff'ff, \
	.base_middle              = (base & 0x00'ff'00'00) >> 16, \
	.base_high                = (base & 0xff'00'00'00) >> 20, \
	\
	.limit_low                = limit & 0x0'ff'ff, \
	.limit_high               = (limit & 0xf'00'00) >> 16, \
	\
	.access = _access, \
	\
	.flags = _flags \
}

// used to define the access byte of a GDT_entry and the flags quartet
enum : u8
{
	// --- access byte ---
	// bit 7
	GDT_ENTRY_ACCESS_PRESENT             = 0b1'0000000,

	// BIT 5-6
	GDT_ENTRY_ACCESS_RING0               = 0b0'00'00000,
	GDT_ENTRY_ACCESS_RING1               = 0b0'01'00000,
	GDT_ENTRY_ACCESS_RING2               = 0b0'10'00000,
	GDT_ENTRY_ACCESS_RING3               = 0b0'11'00000,

	// BIT 3-4
	GDT_ENTRY_ACCESS_SYSTEM              = 0b000'00'000,
	GDT_ENTRY_ACCESS_DATA                = 0b000'10'000,
	GDT_ENTRY_ACCESS_CODE                = 0b000'11'000,

	// BIT 2
	GDT_ENTRY_ACCESS_CODE_CONFORMING     = 0b00000'1'00,
	GDT_ENTRY_ACCESS_DATA_DIRECTION_DOWN = 0b00000'1'00,
	GDT_ENTRY_ACCESS_DATA_DIRECTION_UP   = 0b00000'0'00,

	// BIT 1
	GDT_ENTRY_ACCESS_CODE_READABLE       = 0b000000'1'0,
	GDT_ENTRY_ACCESS_DATA_WRITEABLE      = 0b000000'1'0,

	// BIT 0
	GDT_ENTRY_ACCESS_ACCESSED            = 0b0000000'1,


	// --- FLAGS QUARTET ---
	// BIT 3
	GDT_ENTRY_FLAGS_GRANULARITY1_B       = 0b0'000,
	GDT_ENTRY_FLAGS_GRANULARITY4_K       = 0b1'000,

	// BIT 2-1
	GDT_ENTRY_FLAGS_SEGMENT_WIDTH16_BIT  = 0b0'00'0,
	GDT_ENTRY_FLAGS_SEGMENT_WIDTH32_BIT  = 0b0'10'0,
	GDT_ENTRY_FLAGS_SEGMENT_WIDTH64_BIT  = 0b0'01'0, // don't use for sys segments

	// bit 0 is reserved by the cpu
};

// written in assembly see GDT.asm
[[gnu::cdecl]] void GDT_load_x86(const struct GDT_descriptor* descriptor, const u16 code_segment_start_in_GDT, const u16 data_segment_start_in_GDT);
void GDT_initialize_x86(void);


/* --------------- IDT --------------- */

enum IDT_gate_type : u8
{
	IDT_gate_type_task            = 0b0101, // note that in this case, the offset value is unused and should be set to zero.
	IDT_gate_type16_bit_interrupt = 0b0110,
	IDT_gate_type16_bit_trap      = 0b0111,
	IDT_gate_type32_bit_interrupt = 0b1110,
	IDT_gate_type32_bit_trap      = 0b1111,
};

// an IDT_gate is an entry of the i_d_t
struct [[gnu::packed]] IDT_gate
{
	u16 ISR_address_low;
	u16 _GDT_kernel_code_segment; // hardcoded
	u8 _zero;
	enum IDT_gate_type type : 4;
	u8 _zero2 : 1;
	u8 ring : 2;
	bool present : 1;
	u16 ISR_address_high;
};

// see IDT.asm
void IDT_gate_set(const u8 interrupt, const uintptr_t ISR_address, const enum IDT_gate_type, const u8 ring);
[[gnu::cdecl]] void IDT_load_x86(void);
// void IDT_initialize_x86(void);
