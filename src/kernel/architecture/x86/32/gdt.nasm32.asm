[bits 32]

section .text

global GDT_load
GDT_load:
	push ebp
	mov ebp, esp

	; load GDT
	; TODO: why the fuck put it in eax first???
	mov eax, [ebp + 8]
	lgdt [eax]

	; reload code segment
	mov eax, [ebp + 12]
	push eax
	push .reload_cs
	retf
.reload_cs:

	; reload data segments
	mov ax, [ebp + 16]
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	mov esp, ebp
	pop ebp
	ret

