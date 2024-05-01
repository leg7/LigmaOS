#include <stdio.h>
#include "multiboot1.h"

// TODO: Use correct printf formats once they are implemented
void multiboot_info_print(struct multiboot_info *multiboot_info)
{
	puts("Multiboot info:\n");
	printf("\tflags = %u\n\n", multiboot_info->flags);
	if (multiboot_info->flags & MULTIBOOT_INFO_FLAG_MEMORY) {
		printf("\tmemory_lower = %uKB\n"
	   		 "\tmemory_upper = %uKB\n\n",
			multiboot_info->memory.lower, multiboot_info->memory.upper
		);
	}
	if (multiboot_info->flags & MULTIBOOT_INFO_FLAG_BOOT_DEVICE) {
		// TODO: print partitions 1, 2, and 3
		printf("\tboot_device:\n"
			 "\t\tdrive = %u\n\n",
		   multiboot_info->boot_device.drive
		);
	}
	if (multiboot_info->flags & MULTIBOOT_INFO_FLAG_CMDLINE) {
		printf("\tcmdline = \"%s\"\n\n", multiboot_info->cmdline);
	}
	if (multiboot_info->flags & MULTIBOOT_INFO_FLAG_MODS) {
		// TODO: implement multiboot module printing
		printf("\tMULTIBOOT MODULE PRINTING NOT IMPLEMENTED\n\n");
	}
	if (multiboot_info->flags & MULTIBOOT_INFO_FLAG_AOUT_SYMS)
	{
		// TODO: implement multiboot symbol table printing
		printf("\tMULTIBOOT SYMBOL TABLE PRINTING NOT IMPLEMENTED\n\n");
	}
	if (multiboot_info->flags & MULTIBOOT_INFO_FLAG_ELF_SHDR)
	{
		// TODO: implement multiboot elf info printing
		printf("\tMULTIBOOT KERNEL ELF PRINTING NOT IMPLEMENTED\n\n");
	}
	if (multiboot_info->flags & MULTIBOOT_INFO_FLAG_MEMORY_MAP) {
		// TODO: implement multiboot memory map printing
		printf("\tMULTIBOOT MEMORY MAP PRINTING NOT IMPLEMENTED\n\n");
	}
	if (multiboot_info->flags & MULTIBOOT_INFO_FLAG_DRIVE) {
		// TODO: implement multiboot drive info printing
		printf("\tMULTIBOOT DRIVE INFO PRINTING NOT IMPLEMENTED\n\n");
	}
	if (multiboot_info->flags & MULTIBOOT_INFO_FLAG_CONFIG_TABLE) {
		// TODO: implement multiboot config table printing
		printf("\tMULTIBOOT CONFIG TABLE INFO PRINTING NOT IMPLEMENTED\n\n");
	}
	if (multiboot_info->flags & MULTIBOOT_INFO_FLAG_BOOT_LOADER_NAME) {
		printf("\tboot_loader_name = \"%s\"\n\n", multiboot_info->boot_loader_name);
	}
	if (multiboot_info->flags & MULTIBOOT_INFO_FLAG_APM_TABLE) {
		// TODO: implement multiboot apm table printing
		printf("\tMULTIBOOT APM TABLE INFO PRINTING NOT IMPLEMENTED\n\n");
	}
	if (multiboot_info->flags & MULTIBOOT_INFO_FLAG_VBE) {
		// TODO: implement vbe info printing
		printf("\tMULTIBOOT VBE INFO PRINTING NOT IMPLEMENTED\n\n");
	}
	if (multiboot_info->flags & MULTIBOOT_INFO_FLAG_FRAMEBUFFER) {
		printf("\tframebuffer:\n"
			 "\t\taddress = %u\n"
			 "\t\tpitch = %u\n"
			 "\t\twidth = %u pixels\n"
			 "\t\theight = %u pixels\n"
			 "\t\tbits per pixel = %u\n"
			 "\t\ttype = %u\n",
			multiboot_info->framebuffer.address,
			multiboot_info->framebuffer.pitch,
			multiboot_info->framebuffer.width,
			multiboot_info->framebuffer.height,
			multiboot_info->framebuffer.bpp,
			multiboot_info->framebuffer.type
		);
	}
}
