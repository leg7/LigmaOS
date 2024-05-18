#pragma once
#include <library/types.h>
#include "idt.h"

extern u32 ISRs[256]; // table declared in nasm

struct [[gnu::packed]] ISR_parameters
{
    // in the reverse order they are pushed:
    u32 ds;                                            // data segment pushed by us
    u32 edi, esi, ebp, useless, ebx, edx, ecx, eax;    // pusha
    u32 interrupt, error;                              // we push interrupt, error is pushed automatically (or our dummy)
    u32 eip, cs, eflags, esp, ss;                      // pushed automatically by CPU
};

typedef void (*ISR)(struct ISR_parameters const*);
extern ISR ISR_OVERRIDES[IDT_LENGTH];
