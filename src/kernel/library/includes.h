#pragma once
#include "settings.h"

#ifdef _SETTINGS_GRAPHICS_VBE
#	include <graphics/VBE/interface.h>
#else
#	include <graphics/VGA/vga_text_mode.h>
#endif
