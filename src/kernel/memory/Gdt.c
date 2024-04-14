#include "Gdt.h"

#define GdtEntry(base, limit, m_access, m_flags) { \
	.baseLow                 = base & 0x00'00'FF'FF, \
	.baseMiddle              = (base & 0x00'FF'00'00) >> 16, \
	.baseHigh                = (base & 0xFF'00'00'00) >> 20, \
	\
	.limitLow                = limit & 0x0'FF'FF, \
	.limitHigh               = (limit & 0xF'00'00) >> 16, \
	\
	.access = m_access, \
	\
	.flags = m_flags \
}

struct GdtEntry Gdt1[] = {
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

struct GdtDescriptor Gdt = {
	.gdt = Gdt1, .size = sizeof(Gdt1) - 1,
};

void GdtInitializei686(void)
{
	// 8 and 16 because a GDT entry is 8 bytes
	// so the code entry is located 8 bytes in and the
	// data entry 16 bytes in the table
	GdtLoadi686(&Gdt, 8, 16);
}
