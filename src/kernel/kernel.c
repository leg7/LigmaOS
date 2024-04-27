#if defined(__linux__)
#error "You are not using a cross compiler dumbass"
#endif

#if !defined(__i686__)
#error "This needs to be compiled with an ix86-elf compiler"
#endif

#include "graphics/vga_text_mode.h"
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
	puts("Hey, you. You're finally awake\n");
	__asm("int $255");

	if (multiboot_info->flags & (1 << 12))
	{
		u32 color;
		u32 i;
		void *fb = (void*)multiboot_info->framebuffer.address;

		switch (multiboot_info->framebuffer.type)
		{
			case MULTIBOOT_INFO_FRAMEBUFFER_TYPE_INDEXED:
				{
					unsigned best_distance, distance;
					struct multiboot_color *palette = multiboot_info->framebuffer_palette_address;

					color = 0;
					best_distance = 4*256*256;

					for (i = 0; i < multiboot_info->framebuffer_palette_num_colors; i++)
					{
						distance = (0xff - palette[i].blue) * (0xff - palette[i].blue)
							+ palette[i].red * palette[i].red
							+ palette[i].green * palette[i].green;
						if (distance < best_distance)
						{
							color = i;
							best_distance = distance;
						}
					}
				}
				break;

			case MULTIBOOT_INFO_FRAMEBUFFER_TYPE_RGB:
				color = ((1 << multiboot_info->framebuffer_blue_mask_size) - 1)
					<< multiboot_info->framebuffer_blue_field_position;
				break;

			case MULTIBOOT_INFO_FRAMEBUFFER_TYPE_EGA_TEXT:
				color = '\\' | 0x0100;
				break;

			default:
				color = 0xffffffff;
				break;
		}
		for (i = 0; i < multiboot_info->framebuffer.width
		&& i < multiboot_info->framebuffer.height; i++)
		{
			switch (multiboot_info->framebuffer.bpp)
			{
				case 8:
					{
						u8 *pixel = fb + multiboot_info->framebuffer.pitch * i + i;
						*pixel = color;
					}
					break;
				case 15:
				case 16:
					{
						u16 *pixel = fb + multiboot_info->framebuffer.pitch * i + 2 * i;
						*pixel = color;
					}
					break;
				case 24:
					{
						u32 *pixel = fb + multiboot_info->framebuffer.pitch * i + 3 * i;
						*pixel = (color & 0xffffff) | (*pixel & 0xff000000);
					}
					break;

				case 32:
					{
						u32 *pixel = fb + multiboot_info->framebuffer.pitch * i + 4 * i;
						*pixel = color;
					}
					break;
			}
		}
	}

}


