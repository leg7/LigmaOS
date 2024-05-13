[bits 32]

section .text

global IDT_load
IDT_load:
	lidt [esp + 4]
	ret
