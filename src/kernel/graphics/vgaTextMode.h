#pragma once
#include <types.h>

enum VgaTextModeColor : u8
{
	VgaTextModeColorBlack,
	VgaTextModeColorBlue,
	VgaTextModeColorGreen,
	VgaTextModeColorCyan,
	VgaTextModeColorRed,
	VgaTextModeColorMagenta,
	VgaTextModeColorBrown,
	VgaTextModeColorLightGrey,
	VgaTextModeColorDarkGrey,
	VgaTextModeColorLightBlue,
	VgaTextModeColorLightGreen,
	VgaTextModeColorLightCyan,
	VgaTextModeColorLightRed,
	VgaTextModeColorLightMagenta,
	VgaTextModeColorLightBrown,
	VgaTextModeColorWhite,
};

u8 VgaTextModeAttribute
(const enum VgaTextModeColor foreground, const enum VgaTextModeColor background);
u16 VgaTextModeEntry(const unsigned char uc, const u8 color);

void TerminalInitialize(void);

void TerminalPutEntry(const char c, const u8 color, const u8 column, const u8 row);
void TerminalPutPrompt(void);
void TerminalPutChar(const char c);
void TerminalPutString(const char* string);

void TerminalScrollDown(void);
