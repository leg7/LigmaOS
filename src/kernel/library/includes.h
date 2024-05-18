#pragma once
#include "settings.h"

#ifdef _SETTINGS_GRAPHICS_VBE
#	include <graphics/vbe_graphics.h>
#else
#	include <graphics/vga_text_mode.h>
#endif
