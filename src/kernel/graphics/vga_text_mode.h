#pragma once
#include <library/types.h>

enum VGA_text_mode_color : u8
{
	VGA_text_mode_color_black,
	VGA_text_mode_color_blue,
	VGA_text_mode_color_green,
	VGA_text_mode_color_cyan,
	VGA_text_mode_color_red,
	VGA_text_mode_color_magenta,
	VGA_text_mode_color_brown,
	VGA_text_mode_color_light_grey,
	VGA_text_mode_color_dark_grey,
	VGA_text_mode_color_light_blue,
	VGA_text_mode_color_light_green,
	VGA_text_mode_color_light_cyan,
	VGA_text_mode_color_light_red,
	VGA_text_mode_color_light_magenta,
	VGA_text_mode_color_light_brown,
	VGA_text_mode_color_white,
};

u8 VGA_text_mode_attribute (const enum VGA_text_mode_color foreground, const enum VGA_text_mode_color background);
u16 VGA_text_mode_entry(const unsigned char uc, const u8 color);

void VGA_text_mode_initialize(void);

void VGA_text_mode_put_entry(const char c, const u8 color, const u8 column, const u8 row);
void VGA_text_mode_put_prompt(void);
void VGA_text_mode_put_char(const char c);
void VGA_text_mode_put_string(const char* string);

void VGA_text_mode_scroll_down(void);
