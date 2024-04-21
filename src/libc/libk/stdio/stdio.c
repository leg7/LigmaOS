#include "stdio.h"
#include "../kernel/graphics/vgaTextMode.h"

// TODO: make this respect the standard
int puts(const char *string)
{
	TerminalPutString(string);
	return 1;
}

int putchar(int ch)
{
	TerminalPutChar(ch);
	return 1;
}

static inline void unsignedPrint(unsigned x)
{
	constexpr int8_t stackSize = 32;
	char stack[stackSize]; // should be big enough for any float or 64bit number
	int8_t stackLen = 0;

	while (x != 0) {
		const int modulo = x % 10; // TODO: I hope gcc optimizes this correctly
		x /= 10;

		stack[stackLen++] = modulo;
	}

	while (--stackLen >= 0) {
		putchar('0' + stack[stackLen]);
	}
}

static inline void intPrint(const int x)
{
	if (x < 0) {
		putchar('-');
		unsignedPrint(-x); // TODO: check the assembly of this
	} else {
		unsignedPrint(x);
	}
}

// Assumes 32bit SysV abi
// TODO: buffer it and flush on \n
int printf(const char* restrict format, ...)
{
	int32_t* nextArg = (int32_t*)(&format);

	while (*format != '\0') {
		if (*format == '%') switch (*(++format)) {
			// length
			// specifiers
			case '\0':
				return 1;

			case 'd':
			case 'i':
				intPrint(*(++nextArg));
			break;

			case 'u':
				unsignedPrint(*(++nextArg));
			break;

			case 'o': // TODO
			break;

			case 'x': // TODO
			break;
			case 'X': // TODO
			break;

			case 'f': // TODO
			break;
			case 'F': // TODO
			break;

			case 'e': // TODO
			break;
			case 'E': // TODO
			break;

			case 'g': // TODO
			break;
			case 'G': // TODO
			break;

			case 'a': // TODO
			break;
			case 'A': // TODO
			break;

			case 'c':
				putchar(*(++nextArg));
			break;

			case 's':
				puts((char*)(++nextArg));
			break;

			case 'p': // TODO
			break;

			case 'n': // TODO
			break;

			case '%':
				putchar('%');
			break;
		} else {
			putchar(*format);
		}
		++format;
	}

	return 1;
}
