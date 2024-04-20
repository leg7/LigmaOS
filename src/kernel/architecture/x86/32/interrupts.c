#include "interrupts.h"

constexpr struct GdtEntry Gdt[3] = {
	GdtEntry(0, 0, 0, 0),

	// Code segment spanning all of the addressable memory
	GdtEntry(
		0,
		0xFF'FF'FF,
		GdtEntryAccessPresent | GdtEntryAccessRing0 | GdtEntryAccessCode | GdtEntryAccessCodeReadable,
		GdtEntryFlagsSegmentWidth32Bit | GdtEntryFlagsGranularity4K
	),

	// Data segment spanning all of the addressable memory
	GdtEntry(
		0,
		0xFF'FF'FF,
		GdtEntryAccessPresent | GdtEntryAccessRing0 | GdtEntryAccessData | GdtEntryAccessDataWriteable | GdtEntryAccessAccessed,
		GdtEntryFlagsSegmentWidth32Bit | GdtEntryFlagsGranularity4K
	),
};

struct GdtDescriptor GdtDescriptor = {
	.gdt = Gdt,
	.size = sizeof(Gdt) - 1,
};

void GdtInitializeX86(void)
{
	// 8 and 16 because a GDT entry is 8 bytes
	// so the code entry is located 8 bytes in and the
	// data entry 16 bytes in the table
	GdtLoadX86(&GdtDescriptor, 8, 16);
}

// ----- IDT -----


// #define IdtSize 256
// struct IdtGate Idt[IdtSize];

// To be used with the `lidt` asm function whose argument is a pointer to an IdtDescriptor
// const struct [[gnu::packed]]
// {
// 	const u16 size;
// 	const struct IdtGate *idt;
// } IdtDescriptor = {
// 	.size = (sizeof(struct IdtGate) * IdtSize) - 1,
// 	.idt = Idt
// };
//
// void IdtGateSet(const u8 interrupt, const uintptr_t isrAddress, const enum IdtGateType type, const u8 ring)
// {
// 	Idt[interrupt].isrAddressLow = isrAddress & 0x00'00'FF'FF;
// 	Idt[interrupt].isrAddressHigh = (isrAddress & 0xFF'FF'00'00) >> 16;
//
// 	Idt[interrupt]._gdtKernelCodeSegment = 8;
// 	Idt[interrupt].type = type;
// 	Idt[interrupt].ring = ring;
// 	Idt[interrupt].present = true;
//
// 	Idt[interrupt]._zero = 0;
// 	Idt[interrupt]._zero2 = 0;
// }

// void IdtInitializeX86(void)
// {
// 	IdtGateSet(0,   (uintptr_t)Isr0,   IdtGateType32BitTrap, 0);
// 	IdtLoadX86();
// }
