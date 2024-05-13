#include "isr.h"
#include <stdio.h>

char const reserved[] = "Reserved";

char const* ISR_exception_messages[32] = {
	"Division by 0",
	"Debug",
	"Non-maskable interrupt",
	"Breakpoint",
	"Overflow",
	"Bound range exceeded",
	"Invalid opcode",
	"Device not available",
	"Double fault",
	"Coprocessor segment overrun",
	"Invalid TSS",
	"Segment not present",
	"Stack segment fault",
	"General protection fault",
	"Page fault",
	reserved,
	"x87 floating point exception",
	"Alignment check",
	"Machine check",
	"SIMD floating point exception",
	"Virtualization exception",
	"Control protection exception",
	reserved,
	reserved,
	reserved,
	reserved,
	reserved,
	reserved,
	"Hypervisor injection exception",
	"VMM communication exception",
	"Security exception",
	reserved,
};

struct [[gnu::packed]] ISR_handler_parameters
{
    // in the reverse order they are pushed:
    u32 ds;                                            // data segment pushed by us
    u32 edi, esi, ebp, useless, ebx, edx, ecx, eax;    // pusha
    u32 interrupt, error;                              // we push interrupt, error is pushed automatically (or our dummy)
    u32 eip, cs, eflags, esp, ss;                      // pushed automatically by CPU
};

[[gnu::cdecl]] void ISR_handler(struct ISR_handler_parameters *p)
{
	if (p->interrupt < 32) {
		printf("Exception %d: %s\n", p->interrupt, ISR_exception_messages[p->interrupt]);
	} else {
		printf("Interrupt %d\n", p->interrupt);
	}
}
