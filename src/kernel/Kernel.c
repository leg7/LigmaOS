#if defined(__linux__)
#error "You are not using a cross compiler dumbass"
#endif

#if !defined(__i386__)
#error "This needs to be compiled with an ix86-elf compiler"
#endif

#include "graphics/VgaTextMode.h"
#include "memory/Gdt.h"
#include "memory/Idt.h"

void KernelMain(void)
{
	TerminalInitialize();
	// TerminalPutString("Hello, kernel World! 1\n");
	// TerminalPutString("Hello, kernel World! 2\n");
	// TerminalPutString("Hello, kernel World! 3\n");
	// TerminalPutString("Hello, kernel World! 4\n");
	// TerminalPutString("Hello, kernel World! 5\n");
	// TerminalPutString("Hello, kernel World! 6\n");
	// TerminalPutString("Hello, kernel World! 7\n");
	// TerminalPutString("Hello, kernel World! 8\n");
	// TerminalPutString("Hello, kernel World! 9\n");
	// TerminalPutString("Hello, kernel World! 10\n");
	// TerminalPutString("Hello, kernel World! 11\n");
	// TerminalPutString("Hello, kernel World! 12\n");
	// TerminalPutString("Hello, kernel World! 13\n");
	// TerminalPutString("Hello, kernel World! 14\n");
	// TerminalPutString("Hello, kernel World! 15\n");
	// TerminalPutString("Hello, kernel World! 16\n");
	// TerminalPutString("Hello, kernel World! 17\n");
	// TerminalPutString("Hello, kernel World! 18\n");
	// TerminalPutString("Hello, kernel World! 19\n");
	// TerminalPutString("Hello, kernel World! 20\n");
	// TerminalPutString("Hello, kernel World! 21\n");
	// TerminalPutString("Hello, kernel World! 22\n");
	// TerminalPutString("Hello, kernel World! 23\n");
	// TerminalPutString("Hello, kernel World! 24\n");
	// TerminalPutString("Hello, kernel World! 25\n");
	// TerminalPutString("Hello, kernel World! 26\n");
	// TerminalPutString("Hello, kernel World! 27\n");
	// TerminalPutString("Hello, kernel World! 28\n");
	// TerminalPutString("Hello, kernel World! 29\n");
	// TerminalPutString("Hello, kernel World! 30\n");
	// TerminalPutString("Hello, kernel World! 31\n");
	// TerminalPutString("Hello, kernel World! 32\n");
	// TerminalPutString("Hello, kernel World! 33\n");
	// TerminalPutString("Hello, kernel World! 34\n");
	// TerminalPutString("Hello, kernel World! 35\n");
	// TerminalPutString("Hello, kernel World! 36\n");
	// TerminalPutString("Hello, kernel World! 37\n");
	// TerminalPutString("Hello, kernel World! 38\n");
	// TerminalPutString("Hello, kernel World! 39\n");

	GdtInitializei686();
	IdtInitializei686();
}
