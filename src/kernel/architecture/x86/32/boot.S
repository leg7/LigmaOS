.code32

#define ASM_FILE 1
#include <multiboot/multiboot1.h>

.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

#ifdef __ELF__
#	define AOUT_KLUDGE 0
#else
#	define AOUT_KLUDGE MULTIBOOT_HEADER_FLAG_AOUT_KLUDGE
#endif

#define MULTIBOOT_HEADER_FLAGS 	MULTIBOOT_HEADER_FLAG_PAGE_ALIGN  | MULTIBOOT_HEADER_FLAG_MEMORY_INFO | MULTIBOOT_HEADER_FLAG_VIDEO_MODE  | AOUT_KLUDGE

.section .multiboot
.align 4
multiboot_header:
	.long MULTIBOOT_HEADER_MAGIC
	.long MULTIBOOT_HEADER_FLAGS
	.long -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
#	ifndef __ELF__
		.long multiboot_header
		.long _start
		.long _edata
		.long _end
		.long multiboot_entry
#	else // !__ELF__
		.long 0
		.long 0
		.long 0
		.long 0
		.long 0
#	endif
/* TODO: Don't assume resolution */
	.long 1
	.long 80
	.long 25
	.long 0

.section .text
.global _start
.type _start, @function
_start:
	movl $stack_top, %esp

	// Reset eflags
	pushl $0
	popf

	pushl %ebx
	pushl %eax
	call kernel_main

	pushl $halt_message
	call puts
loop: hlt
	jmp loop

halt_message:
	.asciz "Halted."

