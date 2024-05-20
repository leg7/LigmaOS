#pragma once
#include <library/types.h>
#include <string.h>
#include <multiboot/multiboot1.h>
#include "font_bitmap.h"


void put_char_terminal(char c);

void set_newline();

void put_string_terminal(char *str);

void delete_char_terminal();


void test_interface();





