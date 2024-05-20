#include "stdio.h"
#include <stdint.h>
#include <library/settings.h>
#include <library/includes.h>

// TODO: make this respect the standard
int puts(const char *string)
{
#	ifdef _SETTINGS_GRAPHICS_VBE
		put_string_terminal(string);
#	else
		VGA_text_mode_put_string(string);
#	endif

	return 1;
}

int putchar(int ch)
{
#	ifdef _SETTINGS_GRAPHICS_VBE
		put_char_terminal(ch);
#	else
		VGA_text_mode_put_char(ch);
#	endif

	return 1;
}


// TODO: Factor out stack functionality
static inline void unsigned_print(unsigned x)
{
	if (x == 0) {
		putchar('0');
		return;
	}

	constexpr int8_t stack_size = 32;
	char stack[stack_size]; // should be big enough for any float or 64bit number
	int8_t stack_len = 0;

	while (x != 0) {
		const int modulo = x % 10; // TODO: I hope gcc optimizes this correctly
		x /= 10;

		stack[stack_len++] = modulo;
	}

	while (--stack_len >= 0) {
		putchar('0' + stack[stack_len]);
	}
}

static inline void int_print(int const x)
{
	if (x < 0) {
		putchar('-');
		unsigned_print(-x); // TODO: check the assembly of this
	} else {
		unsigned_print(x);
	}
}

static inline void hex_print_lower(unsigned x)
{
	static constexpr char decimal_to_hex_lower[16] = {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'a', 'b', 'c', 'd', 'e', 'f',
	};

	if (x == 0) {
		putchar('0');
		return;
	}

	constexpr int8_t stack_size = 32;
	char stack[stack_size];
	int8_t stack_len = 0;

	while (x != 0) {
		stack[stack_len++] = decimal_to_hex_lower[x % 16];
		x /= 16;
	}

	while (--stack_len >= 0) {
		putchar(stack[stack_len]);
	}
}

static inline void hex_print_upper(unsigned x)
{
	static constexpr char decimal_to_hex_upper[16] = {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'A', 'B', 'C', 'D', 'E', 'F',
	};

	if (x == 0) {
		putchar('0');
		return;
	}

	constexpr int8_t stack_size = 32;
	char stack[stack_size];
	int8_t stack_len = 0;

	while (x != 0) {
		stack[stack_len++] = decimal_to_hex_upper[x % 16];
		x /= 16;
	}

	while (--stack_len >= 0) {
		putchar(stack[stack_len]);
	}
}

// Assumes 32bit SysV abi
// TODO: buffer it and flush on \n
int printf(const char* restrict format, ...)
{
	int32_t* next_arg = (int32_t*)(&format);

	while (*format != '\0') {
		if (*format == '%') switch (*(++format)) {
			// length
			// specifiers
			case '\0':
				return 1;

			case 'd':
			case 'i':
				int_print(*(++next_arg));
			break;

			case 'u':
				unsigned_print(*(++next_arg));
			break;

			case 'o': // TODO
			break;

			case 'x':
				hex_print_lower(*(++next_arg));
			break;
			case 'X':
				hex_print_upper(*(++next_arg));
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
				putchar(*(++next_arg));
			break;

			case 's':
				puts((char*)(*(++next_arg)));
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
