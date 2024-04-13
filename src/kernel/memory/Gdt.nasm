[bits 32]
global GdtLoadi686

GdtLoadi686:
	push ebp
	mov ebp, esp

	; load gdt
	mov eax, [ebp + 8]
	lgdt [eax]

	; reload code segment
	mov eax, [ebp + 12]
	push eax
	push .reloadCs
	retf

.reloadCs:

	; reload data segments
	mov ax, [ebp + 14]
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	mov esp, ebp
	pop ebp
	ret


