#include "vga_text_mode.h"
#include <string.h>

struct
{
	const u8 width, height;
	u8 row, column;
	u8 color;
	u16* buffer;
}
vga_text_mode = {
	80, 25,
	0, 0,
	VGA_text_mode_color_light_grey | VGA_text_mode_color_black << 4,
	(u16*)(0xb8000),
};

u8 VGA_text_mode_attribute(const enum VGA_text_mode_color foreground, const enum VGA_text_mode_color background)
{
	return foreground | background << 4;
}

u16 VGA_text_mode_entry(const unsigned char uc, const u8 color)
{
	return (u16)(color << 8) | uc;
}

void VGA_text_mode_initialize(void)
{
	VGA_text_mode_put_prompt();
	const u16 entry = VGA_text_mode_entry(' ', vga_text_mode.color);

	// TODO: use memset
	for (u8 y = 1; y < vga_text_mode.height; ++y) {
		for (u8 x = 0; x < vga_text_mode.width; ++x) {
			const u16 index = y * vga_text_mode.width + x;
			vga_text_mode.buffer[index] = entry;
		}
	}
}

void VGA_text_mode_put_entry(const char c, const u8 color, const u8 column, const u8 row)
{
	const u16 index = row * vga_text_mode.width + column;
	vga_text_mode.buffer[index] = VGA_text_mode_entry(c, color);
}

void VGA_text_mode_put_prompt(void)
{
	static constexpr u8 prompt_length = 2;
	static constexpr char prompt[prompt_length] = "$ ";
	const u8 prompt_color = VGA_text_mode_attribute(VGA_text_mode_color_red, VGA_text_mode_color_black);

	for (u16 i = 0; i < prompt_length; ++i) {
		VGA_text_mode_put_entry(prompt[i], prompt_color, i, vga_text_mode.row);
	}
	for (u16 i = prompt_length; i < vga_text_mode.width; ++i) {
		VGA_text_mode_put_entry(' ', vga_text_mode.color, i, vga_text_mode.row);
	}

	vga_text_mode.column = prompt_length;
}

void VGA_text_mode_put_char(const char c)
{
	if (c == '\n' || c == '\r') {
		vga_text_mode.column = 0;
		++vga_text_mode.row;
		VGA_text_mode_put_prompt();
	} else if (c == '\t') {
		static constexpr u8 tab_size = 4;
		vga_text_mode.column += tab_size;

		if (vga_text_mode.column >= vga_text_mode.width) {
			vga_text_mode.column = vga_text_mode.column - vga_text_mode.width;
			++vga_text_mode.row;
		}
	} else {
		VGA_text_mode_put_entry(c, vga_text_mode.color, vga_text_mode.column, vga_text_mode.row);
		++vga_text_mode.column;

		const bool reached_end = vga_text_mode.column == vga_text_mode.width;
		if (reached_end) {
			vga_text_mode.column = 0;
			++vga_text_mode.row;
			VGA_text_mode_put_prompt();
		}
	}


	const bool reached_bottom = vga_text_mode.row == vga_text_mode.height;
	if (reached_bottom) {
		--vga_text_mode.row;
		VGA_text_mode_scroll_down();
	}

}

void VGA_text_mode_put_string(const char* string)
{
	const usize size = strlen(string);
	for (usize i = 0; i < size; ++i) {
		VGA_text_mode_put_char(string[i]);
	}
}

void VGA_text_mode_scroll_down(void)
{
	for (u8 y = 0; y < vga_text_mode.height - 1; ++y) {
		for (u8 x = 0; x < vga_text_mode.width; ++x) {
			const u16 index_current = y * vga_text_mode.width + x;
			const u16 index_below = y * vga_text_mode.width + vga_text_mode.width + x;
			vga_text_mode.buffer[index_current] = vga_text_mode.buffer[index_below];
		}
	}

	VGA_text_mode_put_prompt();
}
