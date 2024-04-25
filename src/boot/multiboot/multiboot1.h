#ifndef MULTIBOOT_HEADER
#define MULTIBOOT_HEADER 1

/* How many bytes from the start of the file we search for the header. */
#define MULTIBOOT_SEARCH                        8192

/* This should be in %eax. */
#define MULTIBOOT_OUTPUT_MAGIC              0x2BADB002

#define MULTIBOOT_MODULES_ALIGNMENT             4096

// ---------- multiboot_header ----------

#define MULTIBOOT_HEADER_ALIGNMENT                   4

/* The magic field should contain this. */
#define MULTIBOOT_HEADER_MAGIC                       0x1BADB002

// Align all boot modules on i386 page (4KB) boundaries.
#define MULTIBOOT_HEADER_FLAG_PAGE_ALIGN             1
#define MULTIBOOT_HEADER_FLAG_MEMORY_INFO            1 << 1
// These flags tell the bootloader what parts of the header are being used
#define MULTIBOOT_HEADER_FLAG_VIDEO_MODE             1 << 2
#define MULTIBOOT_HEADER_FLAG_AOUT_KLUDGE            1 << 16

// ---------- multiboot_info ----------

#define MULTIBOOT_INFO_ALIGNMENT                     4

#define MULTIBOOT_INFO_FLAG_MEMORY                   1
#define MULTIBOOT_INFO_FLAG_BOOT_DEVICE              1 << 1
#define MULTIBOOT_INFO_FLAG_CMDLINE                  1 << 2
#define MULTIBOOT_INFO_FLAG_MODS                     1 << 3
#define MULTIBOOT_INFO_FLAG_AOUT_SYMS                1 << 4 // mutually exclusive with bit 5
#define MULTIBOOT_INFO_FLAG_ELF_SHDR                 1 << 5 // mutually exclusive with bit 4
#define MULTIBOOT_INFO_FLAG_MEMORY_MAP               1 << 6
#define MULTIBOOT_INFO_FLAG_DRIVE                    1 << 7
#define MULTIBOOT_INFO_FLAG_CONFIG_TABLE             1 << 8
#define MULTIBOOT_INFO_FLAG_BOOT_LOADER_NAME         1 << 9
#define MULTIBOOT_INFO_FLAG_APM_TABLE                1 << 10
#define MULTIBOOT_INFO_FLAG_VBE                      1 << 11
#define MULTIBOOT_INFO_FLAG_FRAMEBUFFER              1 << 12

#ifndef ASM_FILE

#include <types.h>

struct [[gnu::packed]] multiboot_header
{
	u32 magic; // must be MULTIBOOT_HEADER_MAGIC
	u32 flags;
	u32 checksum; // The above fields plus this one must equal 0 mod 2^32.

	/* These are only valid if MULTIBOOT_HEADER_FLAG_AOUT_KLUDGE is set. */
	u32 header_address;
	u32 load_address;
	u32 load_end_address;
	u32 bss_end_address;
	u32 entry_address;

	/* These are only valid if MULTIBOOT_HEADER_FLAG_VIDEO_MODE is set. */
	u32 mode_type;
	u32 width;
	u32 height;
	u32 depth;
};

/* The symbol table for a.out. */
struct [[gnu::packed]] multiboot_info_aout_symbol_table
{
	u32 tabsize;
	u32 strsize;
	u32 address;
	u32 reserved;
};

/* The section header table for ELF. */
struct [[gnu::packed]] multiboot_info_elf_section_header_table
{
	u32 num;
	u32 size;
	u32 address;
	u32 shndx;
};

struct [[gnu::packed]] multiboot_info
{
	u32 flags;

	struct [[gnu::packed]] {
		u32 lower;
		u32 upper;
	} memory;

	struct [[gnu::packed]] {
		u8 part3;
		u8 part2;
		u8 part1;
		u8 drive;
	} boot_device;

	char* cmdline;

	struct [[gnu::packed]] {
		u32 count;
		u32 address;
	} modules;

	union
	{
		struct multiboot_info_aout_symbol_table aout_sym;
		struct multiboot_info_elf_section_header_table elf_sec;
	} u;

	struct [[gnu::packed]] {
		u32 length;
		u32 address;
	} memory_map;

	struct [[gnu::packed]] {
		u32 length;
		u32 address;
	} drives;

	u32 config_table;

	char* boot_loader_name;

	u32 apm_table;

	struct [[gnu::packed]] {
		u32 control_info;
		u32 mode_info;
		u16 mode;
		u16 interface_seg;
		u16 interface_off;
		u16 interface_length;
	} vbe;

	struct [[gnu::packed]] {
		u64 address;
		u32 pitch;
		u32 width;
		u32 height;
		u8 bpp;
		u8 type;
	} framebuffer;
#define MULTIBOOT_INFO_FRAMEBUFFER_TYPE_INDEXED   0
#define MULTIBOOT_INFO_FRAMEBUFFER_TYPE_RGB       1
#define MULTIBOOT_INFO_FRAMEBUFFER_TYPE_EGA_TEXT  2

	union
	{
		struct [[gnu::packed]]
		{
			u32 framebuffer_palette_address;
			u16 framebuffer_palette_num_colors;
		};
		struct [[gnu::packed]]
		{
			u8 framebuffer_red_field_position;
			u8 framebuffer_red_mask_size;
			u8 framebuffer_green_field_position;
			u8 framebuffer_green_mask_size;
			u8 framebuffer_blue_field_position;
			u8 framebuffer_blue_mask_size;
		};
	};
};

struct [[gnu::packed]] multiboot_color
{
	u8 red;
	u8 green;
	u8 blue;
};

struct [[gnu::packed]] multiboot_info_memory_map_entry
{
	u32 size;
	u64 address;
	u64 length;
#define MULTIBOOT_INFO_MEMORY_TYPE_AVAILABLE              1
#define MULTIBOOT_INFO_MEMORY_TYPE_RESERVED               2
#define MULTIBOOT_INFO_MEMORY_TYPE_ACPI_RECLAIMABLE       3
#define MULTIBOOT_INFO_MEMORY_TYPE_NVS                    4
#define MULTIBOOT_INFO_MEMORY_TYPE_BADRAM                 5
	u32 type;
};

struct [[gnu::packed]] multiboot_info_module_list
{
	/* the memory used goes from bytes ’mod_start’ to ’mod_end-1’ inclusive */
	u32 mod_start;
	u32 mod_end;

	/* Module command line */
	char* cmdline;

	/* padding to take it to 16 bytes (must be zero) */
	u32 pad;
};

struct [[gnu::packed]] multiboot_info_apm
{
	u16 version;
	u16 cseg;
	u32 offset;
	u16 cseg_16;
	u16 dseg;
	u16 flags;
	u16 cseg_length;
	u16 cseg_16_length;
	u16 dseg_length;
};

void multiboot_info_print(struct multiboot_info *multiboot_info);

#endif /* ! ASM_FILE */

#endif /* ! MULTIBOOT_HEADER */

