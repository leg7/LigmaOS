[bits 32]
global IdtLoadX86
global GdtLoadX86
extern printf

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

		push dword [IsrMessages + (4 * %1)]
		push dword %1
		push dword Msg
		call printf
		add esp, 12

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
	mov word [Idt + eax], dx                  ; Isr address low
	mov word [Idt + eax + 2], 8 		  ; Kernel code segment selector
	mov byte [Idt + eax + 4], 0               ; Reserved byte
	mov byte [Idt + eax + 5], cl              ; Flags byte
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
	Msg: db "Interrupt %i: %s",10,0

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

	IsrMessage0:   db "Division by 0", 0
	IsrMessage1:   db "Debug", 0
	IsrMessage2:   db "Non-maskable interrupt", 0
	IsrMessage3:   db "Breakpoint", 0
	IsrMessage4:   db "Overflow", 0
	IsrMessage5:   db "Bound Range Exceeded", 0
	IsrMessage6:   db "Invalid Opcode", 0
	IsrMessage7:   db "Device not available", 0
	IsrMessage8:   db "Double Fault", 0
	IsrMessage9:   db "Coprocessor Segment Overrun", 0
	IsrMessage10:  db "Invalid TSS", 0
	IsrMessage11:  db "Segment not present", 0
	IsrMessage12:  db "Stack-Segment Fault", 0
	IsrMessage13:  db "General Protection Fault", 0
	IsrMessage14:  db "Page Fault", 0
	IsrMessage15:  db "Reserved", 0
	IsrMessage16:  db "x87 Floating-Point Exception", 0
	IsrMessage17:  db "Alignment Check", 0
	IsrMessage18:  db "Machine Check", 0
	IsrMessage19:  db "SIMD Floating-Point Exception", 0
	IsrMessage20:  db "Virtualization Exception", 0
	IsrMessage21:  db "Control Protection Exception", 0
	IsrMessage22:  db "Reserved", 0
	IsrMessage23:  db "Reserved", 0
	IsrMessage24:  db "Reserved", 0
	IsrMessage25:  db "Reserved", 0
	IsrMessage26:  db "Reserved", 0
	IsrMessage27:  db "Reserved", 0
	IsrMessage28:  db "Hypervisor Injection Exception", 0
	IsrMessage29:  db "VMM Communication Exception", 0
	IsrMessage30:  db "Security Exception", 0
	IsrMessage31:  db "Reserved", 0
	IsrMessage32:  db "Testing for 32",  0
	IsrMessage33:  db "Testing for 33",  0
	IsrMessage34:  db "Testing for 34",  0
	IsrMessage35:  db "Testing for 35",  0
	IsrMessage36:  db "Testing for 36",  0
	IsrMessage37:  db "Testing for 37",  0
	IsrMessage38:  db "Testing for 38",  0
	IsrMessage39:  db "Testing for 39",  0
	IsrMessage40:  db "Testing for 40",  0
	IsrMessage41:  db "Testing for 41",  0
	IsrMessage42:  db "Testing for 42",  0
	IsrMessage43:  db "Testing for 43",  0
	IsrMessage44:  db "Testing for 44",  0
	IsrMessage45:  db "Testing for 45",  0
	IsrMessage46:  db "Testing for 46",  0
	IsrMessage47:  db "Testing for 47",  0
	IsrMessage48:  db "Testing for 48",  0
	IsrMessage49:  db "Testing for 49",  0
	IsrMessage50:  db "Testing for 50",  0
	IsrMessage51:  db "Testing for 51",  0
	IsrMessage52:  db "Testing for 52",  0
	IsrMessage53:  db "Testing for 53",  0
	IsrMessage54:  db "Testing for 54",  0
	IsrMessage55:  db "Testing for 55",  0
	IsrMessage56:  db "Testing for 56",  0
	IsrMessage57:  db "Testing for 57",  0
	IsrMessage58:  db "Testing for 58",  0
	IsrMessage59:  db "Testing for 59",  0
	IsrMessage60:  db "Testing for 60",  0
	IsrMessage61:  db "Testing for 61",  0
	IsrMessage62:  db "Testing for 62",  0
	IsrMessage63:  db "Testing for 63",  0
	IsrMessage64:  db "Testing for 64",  0
	IsrMessage65:  db "Testing for 65",  0
	IsrMessage66:  db "Testing for 66",  0
	IsrMessage67:  db "Testing for 67",  0
	IsrMessage68:  db "Testing for 68",  0
	IsrMessage69:  db "Testing for 69",  0
	IsrMessage70:  db "Testing for 70",  0
	IsrMessage71:  db "Testing for 71",  0
	IsrMessage72:  db "Testing for 72",  0
	IsrMessage73:  db "Testing for 73",  0
	IsrMessage74:  db "Testing for 74",  0
	IsrMessage75:  db "Testing for 75",  0
	IsrMessage76:  db "Testing for 76",  0
	IsrMessage77:  db "Testing for 77",  0
	IsrMessage78:  db "Testing for 78",  0
	IsrMessage79:  db "Testing for 79",  0
	IsrMessage80:  db "Testing for 80",  0
	IsrMessage81:  db "Testing for 81",  0
	IsrMessage82:  db "Testing for 82",  0
	IsrMessage83:  db "Testing for 83",  0
	IsrMessage84:  db "Testing for 84",  0
	IsrMessage85:  db "Testing for 85",  0
	IsrMessage86:  db "Testing for 86",  0
	IsrMessage87:  db "Testing for 87",  0
	IsrMessage88:  db "Testing for 88",  0
	IsrMessage89:  db "Testing for 89",  0
	IsrMessage90:  db "Testing for 90",  0
	IsrMessage91:  db "Testing for 91",  0
	IsrMessage92:  db "Testing for 92",  0
	IsrMessage93:  db "Testing for 93",  0
	IsrMessage94:  db "Testing for 94",  0
	IsrMessage95:  db "Testing for 95",  0
	IsrMessage96:  db "Testing for 96",  0
	IsrMessage97:  db "Testing for 97",  0
	IsrMessage98:  db "Testing for 98",  0
	IsrMessage99:  db "Testing for 99",  0
	IsrMessage100: db "Testing for 100", 0
	IsrMessage101: db "Testing for 101", 0
	IsrMessage102: db "Testing for 102", 0
	IsrMessage103: db "Testing for 103", 0
	IsrMessage104: db "Testing for 104", 0
	IsrMessage105: db "Testing for 105", 0
	IsrMessage106: db "Testing for 106", 0
	IsrMessage107: db "Testing for 107", 0
	IsrMessage108: db "Testing for 108", 0
	IsrMessage109: db "Testing for 109", 0
	IsrMessage110: db "Testing for 110", 0
	IsrMessage111: db "Testing for 111", 0
	IsrMessage112: db "Testing for 112", 0
	IsrMessage113: db "Testing for 113", 0
	IsrMessage114: db "Testing for 114", 0
	IsrMessage115: db "Testing for 115", 0
	IsrMessage116: db "Testing for 116", 0
	IsrMessage117: db "Testing for 117", 0
	IsrMessage118: db "Testing for 118", 0
	IsrMessage119: db "Testing for 119", 0
	IsrMessage120: db "Testing for 120", 0
	IsrMessage121: db "Testing for 121", 0
	IsrMessage122: db "Testing for 122", 0
	IsrMessage123: db "Testing for 123", 0
	IsrMessage124: db "Testing for 124", 0
	IsrMessage125: db "Testing for 125", 0
	IsrMessage126: db "Testing for 126", 0
	IsrMessage127: db "Testing for 127", 0
	IsrMessage128: db "Testing for 128", 0
	IsrMessage129: db "Testing for 129", 0
	IsrMessage130: db "Testing for 130", 0
	IsrMessage131: db "Testing for 131", 0
	IsrMessage132: db "Testing for 132", 0
	IsrMessage133: db "Testing for 133", 0
	IsrMessage134: db "Testing for 134", 0
	IsrMessage135: db "Testing for 135", 0
	IsrMessage136: db "Testing for 136", 0
	IsrMessage137: db "Testing for 137", 0
	IsrMessage138: db "Testing for 138", 0
	IsrMessage139: db "Testing for 139", 0
	IsrMessage140: db "Testing for 140", 0
	IsrMessage141: db "Testing for 141", 0
	IsrMessage142: db "Testing for 142", 0
	IsrMessage143: db "Testing for 143", 0
	IsrMessage144: db "Testing for 144", 0
	IsrMessage145: db "Testing for 145", 0
	IsrMessage146: db "Testing for 146", 0
	IsrMessage147: db "Testing for 147", 0
	IsrMessage148: db "Testing for 148", 0
	IsrMessage149: db "Testing for 149", 0
	IsrMessage150: db "Testing for 150", 0
	IsrMessage151: db "Testing for 151", 0
	IsrMessage152: db "Testing for 152", 0
	IsrMessage153: db "Testing for 153", 0
	IsrMessage154: db "Testing for 154", 0
	IsrMessage155: db "Testing for 155", 0
	IsrMessage156: db "Testing for 156", 0
	IsrMessage157: db "Testing for 157", 0
	IsrMessage158: db "Testing for 158", 0
	IsrMessage159: db "Testing for 159", 0
	IsrMessage160: db "Testing for 160", 0
	IsrMessage161: db "Testing for 161", 0
	IsrMessage162: db "Testing for 162", 0
	IsrMessage163: db "Testing for 163", 0
	IsrMessage164: db "Testing for 164", 0
	IsrMessage165: db "Testing for 165", 0
	IsrMessage166: db "Testing for 166", 0
	IsrMessage167: db "Testing for 167", 0
	IsrMessage168: db "Testing for 168", 0
	IsrMessage169: db "Testing for 169", 0
	IsrMessage170: db "Testing for 170", 0
	IsrMessage171: db "Testing for 171", 0
	IsrMessage172: db "Testing for 172", 0
	IsrMessage173: db "Testing for 173", 0
	IsrMessage174: db "Testing for 174", 0
	IsrMessage175: db "Testing for 175", 0
	IsrMessage176: db "Testing for 176", 0
	IsrMessage177: db "Testing for 177", 0
	IsrMessage178: db "Testing for 178", 0
	IsrMessage179: db "Testing for 179", 0
	IsrMessage180: db "Testing for 180", 0
	IsrMessage181: db "Testing for 181", 0
	IsrMessage182: db "Testing for 182", 0
	IsrMessage183: db "Testing for 183", 0
	IsrMessage184: db "Testing for 184", 0
	IsrMessage185: db "Testing for 185", 0
	IsrMessage186: db "Testing for 186", 0
	IsrMessage187: db "Testing for 187", 0
	IsrMessage188: db "Testing for 188", 0
	IsrMessage189: db "Testing for 189", 0
	IsrMessage190: db "Testing for 190", 0
	IsrMessage191: db "Testing for 191", 0
	IsrMessage192: db "Testing for 192", 0
	IsrMessage193: db "Testing for 193", 0
	IsrMessage194: db "Testing for 194", 0
	IsrMessage195: db "Testing for 195", 0
	IsrMessage196: db "Testing for 196", 0
	IsrMessage197: db "Testing for 197", 0
	IsrMessage198: db "Testing for 198", 0
	IsrMessage199: db "Testing for 199", 0
	IsrMessage200: db "Testing for 200", 0
	IsrMessage201: db "Testing for 201", 0
	IsrMessage202: db "Testing for 202", 0
	IsrMessage203: db "Testing for 203", 0
	IsrMessage204: db "Testing for 204", 0
	IsrMessage205: db "Testing for 205", 0
	IsrMessage206: db "Testing for 206", 0
	IsrMessage207: db "Testing for 207", 0
	IsrMessage208: db "Testing for 208", 0
	IsrMessage209: db "Testing for 209", 0
	IsrMessage210: db "Testing for 210", 0
	IsrMessage211: db "Testing for 211", 0
	IsrMessage212: db "Testing for 212", 0
	IsrMessage213: db "Testing for 213", 0
	IsrMessage214: db "Testing for 214", 0
	IsrMessage215: db "Testing for 215", 0
	IsrMessage216: db "Testing for 216", 0
	IsrMessage217: db "Testing for 217", 0
	IsrMessage218: db "Testing for 218", 0
	IsrMessage219: db "Testing for 219", 0
	IsrMessage220: db "Testing for 220", 0
	IsrMessage221: db "Testing for 221", 0
	IsrMessage222: db "Testing for 222", 0
	IsrMessage223: db "Testing for 223", 0
	IsrMessage224: db "Testing for 224", 0
	IsrMessage225: db "Testing for 225", 0
	IsrMessage226: db "Testing for 226", 0
	IsrMessage227: db "Testing for 227", 0
	IsrMessage228: db "Testing for 228", 0
	IsrMessage229: db "Testing for 229", 0
	IsrMessage230: db "Testing for 230", 0
	IsrMessage231: db "Testing for 231", 0
	IsrMessage232: db "Testing for 232", 0
	IsrMessage233: db "Testing for 233", 0
	IsrMessage234: db "Testing for 234", 0
	IsrMessage235: db "Testing for 235", 0
	IsrMessage236: db "Testing for 236", 0
	IsrMessage237: db "Testing for 237", 0
	IsrMessage238: db "Testing for 238", 0
	IsrMessage239: db "Testing for 239", 0
	IsrMessage240: db "Testing for 240", 0
	IsrMessage241: db "Testing for 241", 0
	IsrMessage242: db "Testing for 242", 0
	IsrMessage243: db "Testing for 243", 0
	IsrMessage244: db "Testing for 244", 0
	IsrMessage245: db "Testing for 245", 0
	IsrMessage246: db "Testing for 246", 0
	IsrMessage247: db "Testing for 247", 0
	IsrMessage248: db "Testing for 248", 0
	IsrMessage249: db "Testing for 249", 0
	IsrMessage250: db "Testing for 250", 0
	IsrMessage251: db "Testing for 251", 0
	IsrMessage252: db "Testing for 252", 0
	IsrMessage253: db "Testing for 253", 0
	IsrMessage254: db "Testing for 254", 0
	IsrMessage255: db "Testing for 255", 0

	IsrMessages: dd IsrMessage0, IsrMessage1, IsrMessage2, IsrMessage3, IsrMessage4, \
		IsrMessage5, IsrMessage6, IsrMessage7, IsrMessage8, IsrMessage9, IsrMessage10, \
		IsrMessage11, IsrMessage12, IsrMessage13, IsrMessage14, IsrMessage15, IsrMessage16, \
		IsrMessage17, IsrMessage18, IsrMessage19, IsrMessage20, IsrMessage21, IsrMessage22, \
		IsrMessage23, IsrMessage24, IsrMessage25, IsrMessage26, IsrMessage27, IsrMessage28, \
		IsrMessage29, IsrMessage30, IsrMessage31, IsrMessage32, IsrMessage33, IsrMessage34, \
		IsrMessage35, IsrMessage36, IsrMessage37, IsrMessage38, IsrMessage39, IsrMessage40, \
		IsrMessage41, IsrMessage42, IsrMessage43, IsrMessage44, IsrMessage45, IsrMessage46, \
		IsrMessage47, IsrMessage48, IsrMessage49, IsrMessage50, IsrMessage51, IsrMessage52, \
		IsrMessage53, IsrMessage54, IsrMessage55, IsrMessage56, IsrMessage57, IsrMessage58, \
		IsrMessage59, IsrMessage60, IsrMessage61, IsrMessage62, IsrMessage63, IsrMessage64, \
		IsrMessage65, IsrMessage66, IsrMessage67, IsrMessage68, IsrMessage69, IsrMessage70, \
		IsrMessage71, IsrMessage72, IsrMessage73, IsrMessage74, IsrMessage75, IsrMessage76, \
		IsrMessage77, IsrMessage78, IsrMessage79, IsrMessage80, IsrMessage81, IsrMessage82, \
		IsrMessage83, IsrMessage84, IsrMessage85, IsrMessage86, IsrMessage87, IsrMessage88, \
		IsrMessage89, IsrMessage90, IsrMessage91, IsrMessage92, IsrMessage93, IsrMessage94, \
		IsrMessage95, IsrMessage96, IsrMessage97, IsrMessage98, IsrMessage99, IsrMessage100, \
		IsrMessage101, IsrMessage102, IsrMessage103, IsrMessage104, IsrMessage105, IsrMessage106, \
		IsrMessage107, IsrMessage108, IsrMessage109, IsrMessage110, IsrMessage111, IsrMessage112, \
		IsrMessage113, IsrMessage114, IsrMessage115, IsrMessage116, IsrMessage117, IsrMessage118, \
		IsrMessage119, IsrMessage120, IsrMessage121, IsrMessage122, IsrMessage123, IsrMessage124, \
		IsrMessage125, IsrMessage126, IsrMessage127, IsrMessage128, IsrMessage129, IsrMessage130, \
		IsrMessage131, IsrMessage132, IsrMessage133, IsrMessage134, IsrMessage135, IsrMessage136, \
		IsrMessage137, IsrMessage138, IsrMessage139, IsrMessage140, IsrMessage141, IsrMessage142, \
		IsrMessage143, IsrMessage144, IsrMessage145, IsrMessage146, IsrMessage147, IsrMessage148, \
		IsrMessage149, IsrMessage150, IsrMessage151, IsrMessage152, IsrMessage153, IsrMessage154, \
		IsrMessage155, IsrMessage156, IsrMessage157, IsrMessage158, IsrMessage159, IsrMessage160, \
		IsrMessage161, IsrMessage162, IsrMessage163, IsrMessage164, IsrMessage165, IsrMessage166, \
		IsrMessage167, IsrMessage168, IsrMessage169, IsrMessage170, IsrMessage171, IsrMessage172, \
		IsrMessage173, IsrMessage174, IsrMessage175, IsrMessage176, IsrMessage177, IsrMessage178, \
		IsrMessage179, IsrMessage180, IsrMessage181, IsrMessage182, IsrMessage183, IsrMessage184, \
		IsrMessage185, IsrMessage186, IsrMessage187, IsrMessage188, IsrMessage189, IsrMessage190, \
		IsrMessage191, IsrMessage192, IsrMessage193, IsrMessage194, IsrMessage195, IsrMessage196, \
		IsrMessage197, IsrMessage198, IsrMessage199, IsrMessage200, IsrMessage201, IsrMessage202, \
		IsrMessage203, IsrMessage204, IsrMessage205, IsrMessage206, IsrMessage207, IsrMessage208, \
		IsrMessage209, IsrMessage210, IsrMessage211, IsrMessage212, IsrMessage213, IsrMessage214, \
		IsrMessage215, IsrMessage216, IsrMessage217, IsrMessage218, IsrMessage219, IsrMessage220, \
		IsrMessage221, IsrMessage222, IsrMessage223, IsrMessage224, IsrMessage225, IsrMessage226, \
		IsrMessage227, IsrMessage228, IsrMessage229, IsrMessage230, IsrMessage231, IsrMessage232, \
		IsrMessage233, IsrMessage234, IsrMessage235, IsrMessage236, IsrMessage237, IsrMessage238, \
		IsrMessage239, IsrMessage240, IsrMessage241, IsrMessage242, IsrMessage243, IsrMessage244, \
		IsrMessage245, IsrMessage246, IsrMessage247, IsrMessage248, IsrMessage249, IsrMessage250, \
		IsrMessage251, IsrMessage252, IsrMessage253, IsrMessage254, IsrMessage255, \

section .bss
	Idt: resq IdtLength

