#pragma once
#include <Types.h>
#include "Gdt.h"

#define IdtSize 256

// To be used with the `lidt` asm function whose argument is a pointer to an IdtDescriptor
struct [[gnu::packed]] IdtDescriptor
{
	const u16 size; // sizeof(Idt) - 1
	const struct IdtGate *idt;
};

enum IdtGateType : u8
{
	IdtGateTypeTask           = 0b0101, // note that in this case, the Offset value is unused and should be set to zero.
	IdtGateType16BitInterrupt = 0b0110,
	IdtGateType16BitTrap      = 0b0111,
	IdtGateType32BitInterrupt = 0b1110,
	IdtGateType32BitTrap      = 0b1111,
};

struct [[gnu::packed]] IdtGate
{
	u16 isrAddressLow;
	struct GdtEntrySelector gdtEntrySelector;
	u8 _zero;
	enum IdtGateType type : 4;
	u8 _zero2 : 1;
	u8 ring : 2;
	bool present : 1;
	u16 isrAddressHigh;
};

// isrAddress u32, segmentSelector
#define IdtGate(isrAddress, _gdtEntrySelector, _type, _ring) \
{ \
	.isrAddressLow = isrAddress & 0x00'00'FF'FF, \
	.isrAddressHigh = (isrAddress & 0xFF'FF'00'00) >> 16, \
	\
	.gdtEntrySelector = _gdtEntrySelector, \
	.type = _type, \
	.ring = _ring, \
	.present = true, \
	\
	._zero = 0, \
	._zero2 = 0, \
}

// See Idt.asm
[[gnu::cdecl]]
void IdtLoadi686(const struct IdtDescriptor* descriptor);

void IdtInitializei686(void);
