#if defined(__linux__)
#error "You are not using a cross compiler, please use the provided nix shell or get a cross compiler at https://github.com/travisg/toolchains"
#endif

#if defined(__x86_64__)
#error "Your compiler is 64 bits and not 32 bits, please use a 32 bit compiler"
#endif

#include <stdio.h>

#include <library/settings.h>
#include <library/includes.h>

#include <multiboot/multiboot1.h>
#include <architecture/x86/chips/PS2_8042.h>
#include <architecture/x86/chips/PIC_8259A.h>
#include <architecture/x86/chips/PIT_8254.h>
#include <architecture/x86/32/gdt.h>
#include <architecture/x86/32/idt.h>
#include <architecture/x86/32/isr.h>
#include <architecture/x86/32/irq.h>
#include <drivers/input/PS2_keyboard.h>
#include <drivers/time/RTC.h>

u32 fb;
u32 pitch;
u32 flags;
u8 bpp;

void kernel_main(const u32 multiboot_output_magic, struct multiboot_info* multiboot_info)
{
	if (multiboot_output_magic != MULTIBOOT_OUTPUT_MAGIC) { // kernel wasn't loaded by a multiboot boot loader
		return;
	}

	fb = (u32)multiboot_info->framebuffer.address;
	pitch=multiboot_info->framebuffer.pitch;
	flags=multiboot_info->flags;
	bpp=multiboot_info->framebuffer.bpp;

	#ifdef _SETTINGS_GRAPHICS_VBE
		VBE_test_interface();
	#else
		VGA_text_mode_initialize();
	#endif

	GDT_initialize();
	IDT_initialize();

	IRQ_initialize();
	PS2_8042_initialize();
	PIC_8259A_mask(0);
	PIT_8254_initialize();
	// PIT_8254_frequency_play(500);
	RTC_initialize();

	for (;;) {
		// PS2_keyboard_
	}
}
