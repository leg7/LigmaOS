[bits 32]

section .text

global IRQ_enable
IRQ_enable:
	sti
	ret

global IRQ_disable
IRQ_disable:
	cli
	ret
