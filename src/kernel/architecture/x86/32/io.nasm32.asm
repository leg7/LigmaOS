[bits 32]
global x86_32_out_8, x86_32_out_16, x86_32_out_32, x86_32_in_8, x86_32_in_16, x86_32_in_32

section .text

x86_32_out_8:
	mov edx, [esp + 4]
	mov eax, [esp + 8]
	out dx, al
	ret

x86_32_out_16:
	mov edx, [esp + 4]
	mov eax, [esp + 8]
	out dx, ax
	ret

x86_32_out_32:
	mov edx, [esp + 4]
	mov eax, [esp + 8]
	out dx, eax
	ret

x86_32_in_8:
	mov edx, [esp + 4]
	in al, dx
	ret

x86_32_in_16:
	mov edx, [esp + 4]
	in ax, dx
	ret

x86_32_in_32:
	mov edx, [esp + 4]
	in eax, dx
	ret

global x86_panic
x86_panic:
	cli
	hlt
