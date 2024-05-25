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

void VBE_put_char_terminal(char c);
void VBE_put_string_terminal(char *str);
void VBE_delete_char_terminal();
void VBE_switch_menu_window();
void VBE_switch_text_input_window();
void VBE_switch_piano_window();
void VBE_switch_image_window();
void VBE_quit();
void VBE_test_interface();
