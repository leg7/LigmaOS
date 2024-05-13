#pragma once
#include <types.h>

/* 32bit GDT table implementation for x86_32
 * although we won't be using segmentation, but virtual memory and paging,
 * setting this up is necessary to handle interrupts because the IDT will use the GDT
 * to look up ISRs.
 * we will configure it so that segments span the whole 4GiB of addressable memory */

enum : u16
{
	GDT_SELECTOR_CODE_KERNEL = 8,
	GDT_SELECTOR_DATA_KERNEL = 16,
};

void GDT_initialize(void);


