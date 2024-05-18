#include "isr.h"
#include <architecture/x86/io.h>
#include <stdio.h>

char const RESERVED[] = "Reserved";

char const* ISR_EXCEPTION_MESSAGES[32] = {
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
	RESERVED,
	"x87 floating point exception",
	"Alignment check",
	"Machine check",
	"SIMD floating point exception",
	"Virtualization exception",
	"Control protection exception",
	RESERVED,
	RESERVED,
	RESERVED,
	RESERVED,
	RESERVED,
	RESERVED,
	"Hypervisor injection exception",
	"VMM communication exception",
	"Security exception",
	RESERVED,
};

ISR ISR_OVERRIDES[IDT_LENGTH] = { nullptr };

// TODO: Reduce indirection by setting IDT gates to good handlers right away
[[gnu::cdecl]] void ISR_dispatcher(struct ISR_parameters const *p)
{
	if (ISR_OVERRIDES[p->interrupt] != nullptr) {
		(ISR_OVERRIDES[p->interrupt])(p);
	} else if (p->interrupt < IDT_REPROGRAMABLE_INTERRUPT_START_INDEX) {
		printf("Unhandeled exception %d: %s\n"
			 "\teax = %x, ebx = %x, ecx = %x, edx = %x, esi = %x, edi = %x\n"
			 "\tesp = %x, ebp = %x, eip = %x, eflags = %x, cs = %x, ds = %x, ss = %x\n"
			 "\tinterrupt = %d, errorcode = %x\n"
	   		 "\tKERNEL PANIC!\n",
		   p->interrupt, ISR_EXCEPTION_MESSAGES[p->interrupt],
		   p->eax, p->ebx, p->ecx, p->edx, p->esi, p->edi,
		   p->esp, p->ebp, p->eip, p->eflags, p->cs, p->ds, p->ss,
		   p->interrupt, p->error);
		x86_panic();
	} else {
		printf("Unhandeled interrupt: %d\n", p->interrupt);
	}
}
