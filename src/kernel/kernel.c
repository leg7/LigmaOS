#if defined(__linux__)
#error "You are not using a cross compiler, please use the provided nix shell or get a cross compiler at https://github.com/travisg/toolchains"
#endif

#if defined(__x86_64__)
#error "Your compiler is 64 bits and not 32 bits, please use a 32 bit compiler"
#endif

#include <stdio.h>
#include "graphics/vbe_graphics.h"
#include <graphics/vga_text_mode.h>
#include <multiboot/multiboot1.h>
#include <architecture/x86/chips/PS2_8042.h>
#include <architecture/x86/chips/PIC_8259A.h>
#include <architecture/x86/32/gdt.h>
#include <architecture/x86/32/idt.h>
#include <architecture/x86/32/isr.h>
#include <architecture/x86/32/irq.h>

void* fb;
u32 pitch;
u32 flags;

void kernel_main(const u32 multiboot_output_magic, struct multiboot_info* multiboot_info)
{
	if (multiboot_output_magic != MULTIBOOT_OUTPUT_MAGIC) { // kernel wasn't loaded by a multiboot boot loader
		return;
	}
	fb = (void*)multiboot_info->framebuffer.address;
	pitch=multiboot_info->framebuffer.pitch;
	flags=multiboot_info->flags;

	terminal_initialize();

	GDT_initialize();
	IDT_initialize();

	IRQ_initialize();
	PIC_8259A_mask(0);
	PS2_8042_initialize();

	for (;;) {
		// printf("%d %d\n", PIC_8259A_pending(), PIC_8259A_processing());
	}
}



