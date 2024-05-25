#pragma once
#include "vbe_graphics.h"
#include "piano.h"
#include "picture_print.h"

enum VBE_window{
    MENU_WINDOW,
    PIANO_WINDOW,
    TEXT_INPUT_WINDOW,
    IMAGE_WINDOW
};

extern enum VBE_window current_window;

void VBE_put_char_terminal(char const c);
void VBE_put_string_terminal(char const *str);
void VBE_delete_char_terminal(void);
void VBE_switch_menu_window(void);
void VBE_switch_text_input_window(void);
void VBE_switch_piano_window(void);
void VBE_switch_image_window(void);
void VBE_quit(void);
void VBE_test_interface(void);
void VBE_set_newline(void);

