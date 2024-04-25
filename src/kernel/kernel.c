#if defined(__linux__)
#error "You are not using a cross compiler dumbass"
#endif

#if !defined(__i686__)
#error "This needs to be compiled with an ix86-elf compiler"
#endif

#include "graphics/vgaTextMode.h"
#include "architecture/x86/32/interrupts.h"
#include <stdio.h>
#include <multiboot1.h>

void kernel_main(const u32 multiboot_output_magic, struct multiboot_info* multiboot_info)
{
	if (multiboot_output_magic != MULTIBOOT_OUTPUT_MAGIC) { // kernel wasn't loaded by a multiboot boot loader
		return;
	}

	GdtInitializeX86();
	IdtLoadX86();
}


