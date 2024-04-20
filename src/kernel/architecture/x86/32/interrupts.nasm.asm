[bits 32]
global IdtLoadX86
global GdtLoadX86
extern puts

section .text

; -------------------- ISRs --------------------

global IsrLoadKernelDataSegment
IsrLoadKernelDataSegment:
	mov eax, 16 ; kernel data segment selector in GDT
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	ret


global IsrEnd
IsrEnd:
	; restore segments
	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	popa
	iret

%macro Isr 1
	global Isr%1
	Isr%1:
		pusha
		xor eax, eax
		mov ax, ds
		push eax
		call IsrLoadKernelDataSegment

		push dword Msg
		call puts
		add esp, 4

		jmp IsrEnd
%endmacro

%assign i 0
%rep 256
	Isr i
	%assign i i+1
%endrep

; -------------------- GDT --------------------

GdtLoadX86:
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
	mov ax, [ebp + 16]
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	mov esp, ebp
	pop ebp
	ret

; -------------------- IDT --------------------

%define IdtGateFlagsTypeTask             0b0_00_0_0101  ; note that in this case, the IsrAddress is unused and should be set to zero.
%define IdtGateFlagsType16BitInterrupt   0b0_00_0_0110
%define IdtGateFlagsType16BitTrap        0b0_00_0_0111
%define IdtGateFlagsType32BitInterrupt   0b0_00_0_1110
%define IdtGateFlagsType32BitTrap        0b0_00_0_1111
%define IdtGateFlagsRing0		 0b0_00_0_0000
%define IdtGateFlagsRing3		 0b0_11_0_0000
%define IdtGateFlagsPresent		 0b1_00_0_0000

; params: index of gate [0-255], IdtGateFlags byte
IdtGateAdd:
	; eax: offset in Idt, edx: Isr address, ecx: IdtGate flags
	mov eax, [esp + 4]
	shl eax, 2		; eax = offset in Isrs
	mov edx, [Isrs + eax]
	mov ecx, [esp + 8]
	shl eax, 1 		; eax = offset in Idt

	; --- IdtGate ---
	mov word [Idt + eax], dx                        ; Isr address low
	mov word [Idt + eax + 2], 8 			; Kernel code segment selector
	mov byte [Idt + eax + 4], 0                     ; Reserved byte
	mov byte [Idt + eax + 5], cl                    ; Flags byte
	; isr address high
	shr edx, 16
	mov word [Idt + eax + 6], dx

	ret


IdtLoadX86:
	push ebp
	mov ebp, esp
	push ebx

	xor ebx, ebx
	push dword (IdtGateFlagsType32BitTrap | IdtGateFlagsRing0 | IdtGateFlagsPresent)
	.forExceptions:
		cmp ebx, 31
		jg .endForExceptions

		push ebx
		call IdtGateAdd
		add esp, 4

		inc ebx
		jmp .forExceptions
	.endForExceptions
	add esp, 4
	push dword (IdtGateFlagsType32BitInterrupt | IdtGateFlagsRing0 | IdtGateFlagsPresent)
	.forInterrupts:
		cmp ebx, IdtLength
		jge .endForInterrupts

		push ebx
		call IdtGateAdd
		add esp, 4

		inc ebx
		jmp .forInterrupts
	.endForInterrupts:
	add esp, 4

	lidt [IdtDescriptor]

	pop ebx
	mov esp, ebp
	pop ebp
	ret

section .rodata
	Msg: db "Interrupt!",10,0
	IdtLength equ 256
	IdtDescriptor: dw (IdtLength * 8) - 1 ; sizeof(Idt) - 1
		       dd Idt
	Isrs: dd Isr0, Isr1, Isr2, Isr3, Isr4, Isr5, Isr6, Isr7, Isr8, Isr9, Isr10, Isr11, Isr12, Isr13, Isr14, \
		Isr15, Isr16, Isr17, Isr18, Isr19, Isr20, Isr21, Isr22, Isr23, Isr24, Isr25, Isr26, Isr27, \
		Isr28, Isr29, Isr30, Isr31, Isr32, Isr33, Isr34, Isr35, Isr36, Isr37, Isr38, Isr39, Isr40, \
		Isr41, Isr42, Isr43, Isr44, Isr45, Isr46, Isr47, Isr48, Isr49, Isr50, Isr51, Isr52, Isr53, \
		Isr54, Isr55, Isr56, Isr57, Isr58, Isr59, Isr60, Isr61, Isr62, Isr63, Isr64, Isr65, Isr66, \
		Isr67, Isr68, Isr69, Isr70, Isr71, Isr72, Isr73, Isr74, Isr75, Isr76, Isr77, Isr78, Isr79, \
		Isr80, Isr81, Isr82, Isr83, Isr84, Isr85, Isr86, Isr87, Isr88, Isr89, Isr90, Isr91, Isr92, \
		Isr93, Isr94, Isr95, Isr96, Isr97, Isr98, Isr99, Isr100, Isr101, Isr102, Isr103, Isr104, Isr105, \
		Isr106, Isr107, Isr108, Isr109, Isr110, Isr111, Isr112, Isr113, Isr114, Isr115, Isr116, Isr117, Isr118, \
		Isr119, Isr120, Isr121, Isr122, Isr123, Isr124, Isr125, Isr126, Isr127, Isr128, Isr129, Isr130, Isr131, \
		Isr132, Isr133, Isr134, Isr135, Isr136, Isr137, Isr138, Isr139, Isr140, Isr141, Isr142, Isr143, Isr144, \
		Isr145, Isr146, Isr147, Isr148, Isr149, Isr150, Isr151, Isr152, Isr153, Isr154, Isr155, Isr156, Isr157, \
		Isr158, Isr159, Isr160, Isr161, Isr162, Isr163, Isr164, Isr165, Isr166, Isr167, Isr168, Isr169, Isr170, \
		Isr171, Isr172, Isr173, Isr174, Isr175, Isr176, Isr177, Isr178, Isr179, Isr180, Isr181, Isr182, Isr183, \
		Isr184, Isr185, Isr186, Isr187, Isr188, Isr189, Isr190, Isr191, Isr192, Isr193, Isr194, Isr195, Isr196, \
		Isr197, Isr198, Isr199, Isr200, Isr201, Isr202, Isr203, Isr204, Isr205, Isr206, Isr207, Isr208, Isr209, \
		Isr210, Isr211, Isr212, Isr213, Isr214, Isr215, Isr216, Isr217, Isr218, Isr219, Isr220, Isr221, Isr222, \
		Isr223, Isr224, Isr225, Isr226, Isr227, Isr228, Isr229, Isr230, Isr231, Isr232, Isr233, Isr234, Isr235, \
		Isr236, Isr237, Isr238, Isr239, Isr240, Isr241, Isr242, Isr243, Isr244, Isr245, Isr246, Isr247, Isr248, \
		Isr249, Isr250, Isr251, Isr252, Isr253, Isr254, Isr255

section .bss
	Idt: resq IdtLength

