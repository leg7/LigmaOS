#include "stdio.h"
#include "../kernel/graphics/vgaTextMode.h"

// TODO: make this respect the standard
int puts(const char *string)
{
	TerminalPutString(string);
	return 1;
}

int printf(const char* restrict format, ...)
{

}
