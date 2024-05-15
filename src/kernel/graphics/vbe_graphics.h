#pragma once
#include <types.h>
#include <string.h>
#include <multiboot1.h>
#include "font_bitmap.h"


#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

#define LATERAL_TERMINAL_BORDER_SIZE 110
#define VERTICAL_TERMINAL_BORDER_SIZE 110

#define TERMINAL_WIDTH SCREEN_WIDTH-2*LATERAL_TERMINAL_BORDER_SIZE
#define TERMINAL_HEIGHT SCREEN_HEIGHT-2*VERTICAL_TERMINAL_BORDER_SIZE
#define TERMINAL_PADDING_Y 6
#define TERMINAL_PADDING_X 3
#define TERMINAL_HEADER_WIDTH TERMINAL_WIDTH
#define TERMINAL_HEADER_HEIGHT CHAR_HEIGHT+2*TERMINAL_PADDING_Y
#define TERMINAL_HEADER_TEXT_Y VERTICAL_TERMINAL_BORDER_SIZE+TERMINAL_PADDING_Y
#define TERMINAL_HEADER_TEXT_X LATERAL_TERMINAL_BORDER_SIZE+TERMINAL_PADDING_X

#define TEXT_START_X LATERAL_TERMINAL_BORDER_SIZE+TERMINAL_PADDING_X+CHAR_WIDTH
#define TEXT_START_Y VERTICAL_TERMINAL_BORDER_SIZE+TERMINAL_HEADER_HEIGHT+TERMINAL_PADDING_Y



#define BANNER_X LATERAL_TERMINAL_BORDER_SIZE // Banner must be above the terminal
#define BANNER_Y VERTICAL_TERMINAL_BORDER_SIZE-68

#define CURSOR_BITMAP_INDEX 63




static enum basic_color{
    WHITE=0x00FFFFFF,
    SILVER=0x00C0C0C0,
    GRAY=0x808080,
    BLACK=0x000000,
    RED=0xFF0000,
    MAROON=0x800000,
    YELLOW=0xFFFF00,
    OLIVE=0x808000,
    LIME=0x00FF00,
    GREEN=0x008000,
    AQUA=0x00FFFF,
    TEAL=0x008080,
    BLUE=0x0000FF,
    NAVY=0x000080,
    FUCHSIA=0xFF00FF,
    PURPLE=0x800080
};

void plot_pixel_32bpp(u32 x,u32 y,u32 color);

void put_banner(char *str,u32 color);


void put_char(char c);

void put_string(char *str);

void change_char_color(u32 new_color);

void put_terminal_header_text(char *str);


void put_terminal(u32 color);

void put_cursor();

void delete_cursor();

void set_newline();

void put_string_terminal(char *str);


void delete_char_terminal();


void scroll_down();

void put_char_terminal(char c);

void put_main_window();





