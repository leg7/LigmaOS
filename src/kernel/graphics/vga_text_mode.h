#pragma once
#include <library/types.h>

enum vga_text_mode_color : u8
{
	vga_text_mode_color_black,
	vga_text_mode_color_blue,
	vga_text_mode_color_green,
	vga_text_mode_color_cyan,
	vga_text_mode_color_red,
	vga_text_mode_color_magenta,
	vga_text_mode_color_brown,
	vga_text_mode_color_light_grey,
	vga_text_mode_color_dark_grey,
	vga_text_mode_color_light_blue,
	vga_text_mode_color_light_green,
	vga_text_mode_color_light_cyan,
	vga_text_mode_color_light_red,
	vga_text_mode_color_light_magenta,
	vga_text_mode_color_light_brown,
	vga_text_mode_color_white,
};

u8 vga_text_mode_attribute
(const enum vga_text_mode_color foreground, const enum vga_text_mode_color background);
u16 vga_text_mode_entry(const unsigned char uc, const u8 color);

void terminal_initialize(void);

void terminal_put_entry(const char c, const u8 color, const u8 column, const u8 row);
void terminal_put_prompt(void);
void terminal_put_char(const char c);
void terminal_put_string(const char* string);

void terminal_scroll_down(void);
