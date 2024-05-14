[bits 64]
global x86_64_out_8, x86_64_out_16, x86_64_out_64, x86_64_in_8, x86_64_in_16, x86_64_in_64

section .text

x86_64_out_8:
	mov rdx, rdi
	mov rax, rsi
	out dx, al
	ret

x86_64_out_16:
	mov rdx, rdi
	mov rax, rsi
	out dx, ax
	ret

x86_64_out_32:
	mov rdx, rdi
	mov rax, rsi
	out dx, eax
	ret

x86_64_in_8:
	mov rbx, rdi
	in al, dx
	ret

x86_64_in_16:
	mov rbx, rdi
	in ax, dx
	ret

x86_64_in_32:
	mov rbx, rdi
	in eax, dx
	ret

global x86_panic
x86_panic:
	cli
	hlt
