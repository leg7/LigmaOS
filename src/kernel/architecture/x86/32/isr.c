#include "isr.h"
#include "idt.h"
#include <architecture/x86/io.h>
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

struct [[gnu::packed]] ISR_dispatcher_parameters
{
    // in the reverse order they are pushed:
    u32 ds;                                            // data segment pushed by us
    u32 edi, esi, ebp, useless, ebx, edx, ecx, eax;    // pusha
    u32 interrupt, error;                              // we push interrupt, error is pushed automatically (or our dummy)
    u32 eip, cs, eflags, esp, ss;                      // pushed automatically by CPU
};

// void (*ISR_handlers)(struct ISR_dispatcher_parameters)[IDT_LENGTH];

// TODO: Reduce indirection by setting IDT gates to good handlers right away
[[gnu::cdecl]] void ISR_dispatcher(struct ISR_dispatcher_parameters *p)
{
	if (p->interrupt < 32) {
		// TODO: Use hex format once implemented
		printf("Unhandeled exception %d: %s\n", p->interrupt, ISR_exception_messages[p->interrupt]);
		printf("  eax=%d  ebx=%d  ecx=%d  edx=%d  esi=%d  edi=%d\n",
		   p->eax, p->ebx, p->ecx, p->edx, p->esi, p->edi);
		printf("  esp=%d  ebp=%d  eip=%d  eflags=%d  cs=%d  ds=%d  ss=%d\n",
		   p->esp, p->ebp, p->eip, p->eflags, p->cs, p->ds, p->ss);
		printf("  interrupt=%d  errorcode=%d\n",
		   p->interrupt, p->error);
		printf("KERNEL PANIC!\n");
		x86_panic();
	} else {
		printf("Unhandeled interrupt %d:\n", p->interrupt);
	}
}


