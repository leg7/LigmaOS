#include "Gdt.h"

constexpr struct GdtEntry Gdt[3] = {
	GdtEntry(0, 0, 0, 0),

	// Code segment spanning all of the addressable memory
	GdtEntry(
		0,
		0xFF'FF'FF'FF,
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

void GdtInitializei686(void)
{
	// 8 and 16 because a GDT entry is 8 bytes
	// so the code entry is located 8 bytes in and the
	// data entry 16 bytes in the table
	GdtLoadi686(&GdtDescriptor, 8, 16);
}

