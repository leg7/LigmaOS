#include <Types.h>

/* 32bit GDT table implementation for i686
 * Although we won't be using segmentation, but virtual memory and paging
 * setting this up is necessary to handle interrupts.
 * We will configure it so that segments span the whole 4GiB of addressable memory */

struct [[gnu::packed]] GdtEntry
{
	u16 limitLow;
	u16 baseLow;
	u8 baseMiddle;
	u8 access; 		// defined with bitwise OR operations on the members of the anonymous enum below
	u8 limitHigh : 4;
	u8 flags : 4;	// defined like access
	u8 baseHigh;
};

// Used to define the access byte of a GdtEntry and the flags quartet
enum : u8
{
	// --- Access byte ---
	// bit 7
	GdtEntryAccessPresent           = 0b1'0000000,

	// bit 5-6
	GdtEntryAccessRing0             = 0b0'00'00000,
	GdtEntryAccessRing1             = 0b0'01'00000,
	GdtEntryAccessRing2             = 0b0'10'00000,
	GdtEntryAccessRing3             = 0b0'11'00000,

	// bit 3-4
	GdtEntryAccessSystem            = 0b000'00'000,
	GdtEntryAccessData              = 0b000'10'000,
	GdtEntryAccessCode              = 0b000'11'000,

	// bit 2
	GdtEntryAccessCodeConforming    = 0b00000'1'00,
	GdtEntryAccessDataDirectionDown = 0b00000'1'00,
	GdtEntryAccessDataDirectionUp   = 0b00000'0'00,

	// bit 1
	GdtEntryAccessCodeReadable      = 0b000000'1'0,
	GdtEntryAccessDataWriteable     = 0b000000'1'0,

	// bit 0
	GdtEntryAccessAccessed          = 0b0000000'1,


	// --- Flags quartet ---

	// bit 3
	GdtEntryFlagsGranularity1B     = 0b0'000,
	GdtEntryFlagsGranularity4K     = 0b1'000,

	// bit 2-1
	GdtEntryFlagsSegmentWidth16Bit = 0b0'00'0,
	GdtEntryFlagsSegmentWidth32Bit = 0b0'10'0,
	GdtEntryFlagsSegmentWidth64Bit = 0b0'01'0, // don't use for sys segments

	// bit 0 is reserved by the cpu
};


// To be used with the `lgdt` function in asm whose arument is a pointer to a GDT descriptor
struct [[gnu::packed]] GdtDescriptor
{
	const u16 size; 		// sizeof(gdt) - 1
	const struct GdtEntry* gdt;
};

// Written in assembly see Gdt.asm
[[gnu::cdecl]]
void GdtLoadi686(const struct GdtDescriptor* descriptor, const u16 codeSegmentStartInGdt, const u16 dataSegmentStartinGdt);

void GdtInitializei686(void);

