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

void kernel_main(const u32 multiboot_output_magic, struct multiboot_info* multiboot_info)
{
	if (multiboot_output_magic != MULTIBOOT_OUTPUT_MAGIC) { // kernel wasn't loaded by a multiboot boot loader
		return;
	}

	gdt_initialize_x86();
	idt_load_x86();

	terminal_initialize();
	//puts("Hey, you. You're finally awake\n");
	__asm("int $255");

	//if (multiboot_info->flags & (1 << 12)){
    struct graphics_info graphics_info= graphics_info_init(multiboot_info);
    switch_color_32bpp(&graphics_info,FUCHSIA);

    put_banner(multiboot_info,graphics_info,"  LigmaOS    ");
    graphics_info.cursor_x=8;
    graphics_info.cursor_y=80;
    switch_color_32bpp(&graphics_info,RED);
    put_string(multiboot_info,graphics_info,"  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there !  Hello there ! ");
    
    
    
 
}



