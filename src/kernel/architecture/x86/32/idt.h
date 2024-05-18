#pragma once
#include <types.h>

enum : u16
{
	IDT_LENGTH = 256,
};

enum IDT_gate_type: u8
{
	IDT_GATE_TYPE_TASK             = 0b0101,
	IDT_GATE_TYPE_INTERRUPT_32_BIT = 0b1110,
	IDT_GATE_TYPE_INTERRUPT_16_BIT = 0b0110,
	IDT_GATE_TYPE_TRAP_32_BIT      = 0b1111,
	IDT_GATE_TYPE_TRAP_16_BIT      = 0b0111,
};

enum : u8
{
	IDT_REPROGRAMABLE_INTERRUPT_START_INDEX = 32
};

void IDT_initialize(void);
void IDT_set_gate(u8 const interrupt, u32 const ISR_address, enum IDT_gate_type const type, u8 ring);
