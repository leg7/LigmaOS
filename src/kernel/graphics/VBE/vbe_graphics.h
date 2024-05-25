#pragma once
#include <library/types.h>
#include <string.h>
#include <multiboot/multiboot1.h>
#include "font_bitmap.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

static enum VBE_basic_color{
    WHITE=0x00FFFFFF,
    SILVER=0x00C0C0C0,
    GRAY=0x00808080,
    BLACK=0x00000000,
    RED=0x00FF0000,
    MAROON=0x00800000,
    YELLOW=0x00FFFF00,
    OLIVE=0x00808000,
    LIME=0x0000FF00,
    GREEN=0x00008000,
    AQUA=0x0000FFFF,
    TEAL=0x00008080,
    BLUE=0x000000FF,
    NAVY=0x00000080,
    FUCHSIA=0x00FF00FF,
    PURPLE=0x00800080
};

extern int font8x8_basic[95][8];
extern void* fb;
extern u32 pitch;
extern u8 bpp;

void VBE_plot_pixel_32bpp(u16 x,u16 y,u32 color);

void VBE_put_char(char c,u16 x,u16 y,u32 color);

void VBE_delete_char(u16 x,u16 y);





