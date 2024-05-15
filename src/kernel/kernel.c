#if defined(__linux__)
#error "You are not using a cross compiler dumbass"
#endif

#if !defined(__i686__)
#error "This needs to be compiled with an ix86-elf compiler"
#endif

#include "graphics/vga_text_mode.h"
#include "graphics/vbe_graphics.h"
#include "architecture/x86/32/interrupts.h"
#include <stdio.h>
#include <multiboot1.h>

void* fb;
u32 pitch;
u32 flags;


void kernel_main(const u32 multiboot_output_magic, struct multiboot_info* multiboot_info)
{
	if (multiboot_output_magic != MULTIBOOT_OUTPUT_MAGIC) { // kernel wasn't loaded by a multiboot boot loader
		return;
	}

	gdt_initialize_x86();
	idt_load_x86();

	terminal_initialize();
	__asm("int $255");
    
    fb = (void*)multiboot_info->framebuffer.address;
    pitch=multiboot_info->framebuffer.pitch;
    flags=multiboot_info->flags;
    
    
    put_main_window();
}



