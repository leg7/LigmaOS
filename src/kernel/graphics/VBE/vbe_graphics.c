#include "vbe_graphics.h"

void VBE_plot_pixel_32bpp(u16 x,u16 y,u32 color){
    if ((x<=SCREEN_WIDTH) & (y<=SCREEN_HEIGHT)){
        u32 *pixel = (u8*)fb + pitch * y + (bpp/8) * x;
        *pixel = color;
    }
}

void VBE_put_char(char c,u16 x,u16 y,u32 color){
    u8 row, col;
    for (row = 0; row < CHAR_WIDTH; row++) {
        for (col = 0; col < CHAR_HEIGHT; col++) {
            if (font8x8_basic[c+ASCII_BITMAP_SHIFT][row] & (1 << col)) {
                VBE_plot_pixel_32bpp(col+x,row+y,color);
            }
        }
    }
}

void VBE_delete_char(u16 x,u16 y){
    for (u8 row = 0; row < CHAR_WIDTH; row++) {
        for (u8 col = 0; col < CHAR_HEIGHT; col++) {
            VBE_plot_pixel_32bpp(col+x,row+y,BLACK);
        }
    }
}















