#pragma once
#include <types.h>
#include <string.h>
#include <multiboot1.h>
#include "font_bitmap.h"

struct graphics_info{
    u32 color;
    void * fb;
    u16 cursor_x;
    u16 cursor_y;
    u16 screen_width;
    u16 screen_height;
};

enum basic_color{
    WHITE,
    SILVER,
    GRAY,
    BLACK,
    RED,
    MAROON,
    YELLOW,
    OLIVE,
    LIME,
    GREEN,
    AQUA,
    TEAL,
    BLUE,
    NAVY,
    FUCHSIA,
    PURPLE
};

struct graphics_info graphics_info_init(struct multiboot_info* multiboot_info);

void plot_diagonal_line(struct multiboot_info * multiboot_info,struct graphics_info graphics_info);

void plot_pixel_8bpp(struct multiboot_info * multiboot_info,struct graphics_info graphics_info,u32 x,u32 y);
void plot_pixel_15bpp(struct multiboot_info * multiboot_info,struct graphics_info graphics_info,u32 x,u32 y);
void plot_pixel_16bpp(struct multiboot_info * multiboot_info,struct graphics_info graphics_info,u32 x,u32 y);
void plot_pixel_24bpp(struct multiboot_info * multiboot_info,struct graphics_info graphics_info,u32 x,u32 y);
void plot_pixel_32bpp(struct multiboot_info * multiboot_info,struct graphics_info graphics_info,u32 x,u32 y);

void switch_color_32bpp(struct graphics_info* graphics_info,enum basic_color color);

u8 get_index_from_letter(char c);

void put_char(struct multiboot_info * multiboot_info,struct graphics_info* graphics_info,char c);

void put_char_banner(struct multiboot_info * multiboot_info,struct graphics_info* graphics_info,char c);

void put_string(struct multiboot_info * multiboot_info,struct graphics_info graphics_info,char *str);

void put_banner(struct multiboot_info * multiboot_info,struct graphics_info graphics_info,char *str);




