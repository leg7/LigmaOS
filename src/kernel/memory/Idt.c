#include "Idt.h"

constexpr struct IdtGate Idt[IdtSize] = {
	IdtGate(0, GdtEntrySelector(1, 0), IdtGateType32BitInterrupt, 0)
};

struct IdtDescriptor IdtDescriptor = {
	.size = sizeof(Idt) - 1,
	.idt = Idt
};

void IdtInitializei686(void)
{
	IdtLoadi686(&IdtDescriptor);
}
