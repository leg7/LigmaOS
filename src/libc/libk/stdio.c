#include "stdio.h"
#include "../kernel/graphics/vga_text_mode.h"

// TODO: make this respect the standard
int puts(const char *string)
{
	terminal_put_string(string);
	return 1;
}

int putchar(int ch)
{
	terminal_put_char(ch);
	return 1;
}

static inline void unsignedPrint(unsigned x)
{
	if (x == 0) {
		putchar('0');
		return;
	}

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

enum : uint8_t
{
	PrintfStateLength_none,
	PrintfStateLength_hh,
	PrintfStateLength_h,
	PrintfStateLength_l,
	PrintfStateLength_ll,
	PrintfStateLength_j,
	PrintfStateLength_z,
	PrintfStateLength_t,
	PrintfStateLength_L,
	PrintfStateSpecifier_none,
	PrintfStateSpecifier_d,
	PrintfStateSpecifier_i,
	PrintfStateSpecifier_u,
	PrintfStateSpecifier_o,
	PrintfStateSpecifier_x,
	PrintfStateSpecifier_X,
	PrintfStateSpecifier_f,
	PrintfStateSpecifier_F,
	PrintfStateSpecifier_e,
	PrintfStateSpecifier_E,
	PrintfStateSpecifier_g,
	PrintfStateSpecifier_G,
	PrintfStateSpecifier_a,
	PrintfStateSpecifier_A,
	PrintfStateSpecifier_c,
	PrintfStateSpecifier_s,
	PrintfStateSpecifier_p,
	PrintfStateSpecifier_n,
	PrintfStateCount,
} PrintfState;

// enum PrintfState PrintfStateLength_Transition(const enum PrinfState s)

// constexpr enum PrintfState PrintfStateTransitionTable[PrintfStateCount] = {
//
// };

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
				puts((char*)(*(++nextArg)));
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
