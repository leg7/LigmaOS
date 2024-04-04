#include "VgaTextMode.h"
#include "../libc/string/string.h"

struct
{
	const u8 width, height;
	u8 row, column;
	u8 color;
	u16* buffer;
}
VgaTextMode = {
	80, 25,
	0, 0,
	VgaTextModeColorLightGrey | VgaTextModeColorBlack << 4,
	(u16*)(0xB8000),
};


u8 VgaTextModeAttribute
(const enum VgaTextModeColor foreground, const enum VgaTextModeColor background)
{
	return foreground | background << 4;
}

u16 VgaTextModeEntry(const unsigned char uc, const u8 color)
{
	return (u16)(color << 8) | uc;
}

void TerminalInitialize(void)
{
	TerminalPutPrompt();
	const u16 entry = VgaTextModeEntry(' ', VgaTextMode.color);

	// TODO: Use memset
	for (u8 y = 1; y < VgaTextMode.height; ++y) {
		for (u8 x = 0; x < VgaTextMode.width; ++x) {
			const u16 index = y * VgaTextMode.width + x;
			VgaTextMode.buffer[index] = entry;
		}
	}
}

void TerminalPutEntry(const char c, const u8 color, const u8 column, const u8 row)
{
	const u16 index = row * VgaTextMode.width + column;
	VgaTextMode.buffer[index] = VgaTextModeEntry(c, color);
}

void TerminalPutPrompt(void)
{
	static const char* prompt = "$ ";
	const u8 promptColor = VgaTextModeAttribute(VgaTextModeColorRed, VgaTextModeColorBlack);
	static const u8 promptLength = 2;

	for (u16 i = 0; i < promptLength; ++i) {
		TerminalPutEntry(prompt[i], promptColor, i, VgaTextMode.row);
	}
	for (u16 i = promptLength; i < VgaTextMode.width; ++i) {
		TerminalPutEntry(' ', VgaTextMode.color, i, VgaTextMode.row);
	}

	VgaTextMode.column = promptLength;
}


void TerminalPutChar(const char c)
{
	if (c == '\n' || c == '\r') {
		VgaTextMode.column = 0;
		++VgaTextMode.row;
		TerminalPutPrompt();
	} else {
		TerminalPutEntry(c, VgaTextMode.color, VgaTextMode.column, VgaTextMode.row);
		++VgaTextMode.column;

		const bool reachedEnd = VgaTextMode.column == VgaTextMode.width;
		if (reachedEnd) {
			VgaTextMode.column = 0;
			++VgaTextMode.row;
			TerminalPutPrompt();
		}
	}


	const bool reachedBottom = VgaTextMode.row == VgaTextMode.height;
	if (reachedBottom) {
		--VgaTextMode.row;
		TerminalScrollDown();
	}

}

void TerminalPutString(const char* string)
{
	const usize size = strlen(string);
	for (usize i = 0; i < size; ++i) {
		TerminalPutChar(string[i]);
	}
}

void TerminalScrollDown()
{
	for (u8 y = 0; y < VgaTextMode.height - 1; ++y) {
		for (u8 x = 0; x < VgaTextMode.width; ++x) {
			const u16 indexCurrent = y * VgaTextMode.width + x;
			const u16 indexBelow = y * VgaTextMode.width + VgaTextMode.width + x;
			VgaTextMode.buffer[indexCurrent] = VgaTextMode.buffer[indexBelow];
		}
	}

	TerminalPutPrompt();
}
