[bits 32]
global idt_load_x86
global gdt_load_x86
extern printf

section .text

; -------------------- ISRs --------------------

; TODO: these don't need to be global

global isr_load_kernel_data_segment
isr_load_kernel_data_segment:
	mov eax, 16 ; kernel data segment selector in g_d_t
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	ret


global isr_end
isr_end:
	; restore segments
	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	popa
	iret

%macro isr 1
	global isr%1
	isr%1:
		pusha
		xor eax, eax
		mov ax, ds
		push eax
		call isr_load_kernel_data_segment

		push dword [isr_messages + (4 * %1)]
		push dword %1
		push dword isr_message_intro
		call printf
		add esp, 12

		jmp isr_end
%endmacro

%assign i 0
%rep 256
	isr i
	%assign i i+1
%endrep

; -------------------- GDT --------------------

gdt_load_x86:
	push ebp
	mov ebp, esp

	; load gdt
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

; -------------------- IDT --------------------

%define idt_gate_flags_type_task            0b0_00_0_0101 ; note that in this case, the isr_address is unused and should be set to zero.
%define idt_gate_flags_type16_bit_interrupt 0b0_00_0_0110
%define idt_gate_flags_type16_bit_trap      0b0_00_0_0111
%define idt_gate_flags_type32_bit_interrupt 0b0_00_0_1110
%define idt_gate_flags_type32_bit_trap      0b0_00_0_1111
%define idt_gate_flags_ring0		        0b0_00_0_0000
%define idt_gate_flags_ring3		        0b0_11_0_0000
%define idt_gate_flags_present		  0b1_00_0_0000

; params: index of gate [0-255], idt_gate_flags byte
idt_gate_add:
	; eax: offset in idt, edx: isr address, ecx: idt_gate flags
	mov eax, [esp + 4]
	shl eax, 2		; eax = offset in isrs
	mov edx, [isrs + eax]
	mov ecx, [esp + 8]
	shl eax, 1 		; eax = offset in idt

	; --- idt_gate ---
	mov word [idt + eax], dx                  ; isr address low
	mov word [idt + eax + 2], 8 		  ; kernel code segment selector
	mov byte [idt + eax + 4], 0               ; reserved byte
	mov byte [idt + eax + 5], cl              ; flags byte
	; isr address high
	shr edx, 16
	mov word [idt + eax + 6], dx

	ret


idt_load_x86:
	push ebp
	mov ebp, esp
	push ebx

	xor ebx, ebx
	push dword (idt_gate_flags_type32_bit_trap | idt_gate_flags_ring0 | idt_gate_flags_present)
	.for_exceptions:
		cmp ebx, 31
		jg .end_for_exceptions

		push ebx
		call idt_gate_add
		add esp, 4

		inc ebx
		jmp .for_exceptions
	.end_for_exceptions:
	add esp, 4

	push dword (idt_gate_flags_type32_bit_interrupt | idt_gate_flags_ring0 | idt_gate_flags_present)
	.for_interrupts:
		cmp ebx, idt_length
		jge .end_for_interrupts

		push ebx
		call idt_gate_add
		add esp, 4

		inc ebx
		jmp .for_interrupts
	.end_for_interrupts:
	add esp, 4

	lidt [idt_descriptor]

	pop ebx
	mov esp, ebp
	pop ebp
	ret

section .rodata
	isr_message_intro: db "interrupt %i: %s",10,0

	idt_length equ 256

	idt_descriptor: dw (idt_length * 8) - 1 ; sizeof(idt) - 1
		       dd idt

	isrs: dd isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7, isr8, isr9, isr10, isr11, isr12, isr13, isr14, \
		isr15, isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23, isr24, isr25, isr26, isr27, \
		isr28, isr29, isr30, isr31, isr32, isr33, isr34, isr35, isr36, isr37, isr38, isr39, isr40, \
		isr41, isr42, isr43, isr44, isr45, isr46, isr47, isr48, isr49, isr50, isr51, isr52, isr53, \
		isr54, isr55, isr56, isr57, isr58, isr59, isr60, isr61, isr62, isr63, isr64, isr65, isr66, \
		isr67, isr68, isr69, isr70, isr71, isr72, isr73, isr74, isr75, isr76, isr77, isr78, isr79, \
		isr80, isr81, isr82, isr83, isr84, isr85, isr86, isr87, isr88, isr89, isr90, isr91, isr92, \
		isr93, isr94, isr95, isr96, isr97, isr98, isr99, isr100, isr101, isr102, isr103, isr104, isr105, \
		isr106, isr107, isr108, isr109, isr110, isr111, isr112, isr113, isr114, isr115, isr116, isr117, isr118, \
		isr119, isr120, isr121, isr122, isr123, isr124, isr125, isr126, isr127, isr128, isr129, isr130, isr131, \
		isr132, isr133, isr134, isr135, isr136, isr137, isr138, isr139, isr140, isr141, isr142, isr143, isr144, \
		isr145, isr146, isr147, isr148, isr149, isr150, isr151, isr152, isr153, isr154, isr155, isr156, isr157, \
		isr158, isr159, isr160, isr161, isr162, isr163, isr164, isr165, isr166, isr167, isr168, isr169, isr170, \
		isr171, isr172, isr173, isr174, isr175, isr176, isr177, isr178, isr179, isr180, isr181, isr182, isr183, \
		isr184, isr185, isr186, isr187, isr188, isr189, isr190, isr191, isr192, isr193, isr194, isr195, isr196, \
		isr197, isr198, isr199, isr200, isr201, isr202, isr203, isr204, isr205, isr206, isr207, isr208, isr209, \
		isr210, isr211, isr212, isr213, isr214, isr215, isr216, isr217, isr218, isr219, isr220, isr221, isr222, \
		isr223, isr224, isr225, isr226, isr227, isr228, isr229, isr230, isr231, isr232, isr233, isr234, isr235, \
		isr236, isr237, isr238, isr239, isr240, isr241, isr242, isr243, isr244, isr245, isr246, isr247, isr248, \
		isr249, isr250, isr251, isr252, isr253, isr254, isr255

	isr_message_0:   db "division by 0", 0
	isr_message_1:   db "debug", 0
	isr_message_2:   db "non-maskable interrupt", 0
	isr_message_3:   db "breakpoint", 0
	isr_message_4:   db "overflow", 0
	isr_message_5:   db "bound range exceeded", 0
	isr_message_6:   db "invalid opcode", 0
	isr_message_7:   db "device not available", 0
	isr_message_8:   db "double fault", 0
	isr_message_9:   db "coprocessor segment overrun", 0
	isr_message_10:  db "invalid t_s_s", 0
	isr_message_11:  db "segment not present", 0
	isr_message_12:  db "stack-_segment fault", 0
	isr_message_13:  db "general protection fault", 0
	isr_message_14:  db "page fault", 0
	isr_message_15:  db "reserved", 0
	isr_message_16:  db "x87 floating-_point exception", 0
	isr_message_17:  db "alignment check", 0
	isr_message_18:  db "machine check", 0
	isr_message_19:  db "s_i_m_d floating-_point exception", 0
	isr_message_20:  db "virtualization exception", 0
	isr_message_21:  db "control protection exception", 0
	isr_message_22:  db "reserved", 0
	isr_message_23:  db "reserved", 0
	isr_message_24:  db "reserved", 0
	isr_message_25:  db "reserved", 0
	isr_message_26:  db "reserved", 0
	isr_message_27:  db "reserved", 0
	isr_message_28:  db "hypervisor injection exception", 0
	isr_message_29:  db "v_m_m communication exception", 0
	isr_message_30:  db "security exception", 0
	isr_message_31:  db "reserved", 0
	isr_message_32:  db "testing for 32",  0
	isr_message_33:  db "testing for 33",  0
	isr_message_34:  db "testing for 34",  0
	isr_message_35:  db "testing for 35",  0
	isr_message_36:  db "testing for 36",  0
	isr_message_37:  db "testing for 37",  0
	isr_message_38:  db "testing for 38",  0
	isr_message_39:  db "testing for 39",  0
	isr_message_40:  db "testing for 40",  0
	isr_message_41:  db "testing for 41",  0
	isr_message_42:  db "testing for 42",  0
	isr_message_43:  db "testing for 43",  0
	isr_message_44:  db "testing for 44",  0
	isr_message_45:  db "testing for 45",  0
	isr_message_46:  db "testing for 46",  0
	isr_message_47:  db "testing for 47",  0
	isr_message_48:  db "testing for 48",  0
	isr_message_49:  db "testing for 49",  0
	isr_message_50:  db "testing for 50",  0
	isr_message_51:  db "testing for 51",  0
	isr_message_52:  db "testing for 52",  0
	isr_message_53:  db "testing for 53",  0
	isr_message_54:  db "testing for 54",  0
	isr_message_55:  db "testing for 55",  0
	isr_message_56:  db "testing for 56",  0
	isr_message_57:  db "testing for 57",  0
	isr_message_58:  db "testing for 58",  0
	isr_message_59:  db "testing for 59",  0
	isr_message_60:  db "testing for 60",  0
	isr_message_61:  db "testing for 61",  0
	isr_message_62:  db "testing for 62",  0
	isr_message_63:  db "testing for 63",  0
	isr_message_64:  db "testing for 64",  0
	isr_message_65:  db "testing for 65",  0
	isr_message_66:  db "testing for 66",  0
	isr_message_67:  db "testing for 67",  0
	isr_message_68:  db "testing for 68",  0
	isr_message_69:  db "testing for 69",  0
	isr_message_70:  db "testing for 70",  0
	isr_message_71:  db "testing for 71",  0
	isr_message_72:  db "testing for 72",  0
	isr_message_73:  db "testing for 73",  0
	isr_message_74:  db "testing for 74",  0
	isr_message_75:  db "testing for 75",  0
	isr_message_76:  db "testing for 76",  0
	isr_message_77:  db "testing for 77",  0
	isr_message_78:  db "testing for 78",  0
	isr_message_79:  db "testing for 79",  0
	isr_message_80:  db "testing for 80",  0
	isr_message_81:  db "testing for 81",  0
	isr_message_82:  db "testing for 82",  0
	isr_message_83:  db "testing for 83",  0
	isr_message_84:  db "testing for 84",  0
	isr_message_85:  db "testing for 85",  0
	isr_message_86:  db "testing for 86",  0
	isr_message_87:  db "testing for 87",  0
	isr_message_88:  db "testing for 88",  0
	isr_message_89:  db "testing for 89",  0
	isr_message_90:  db "testing for 90",  0
	isr_message_91:  db "testing for 91",  0
	isr_message_92:  db "testing for 92",  0
	isr_message_93:  db "testing for 93",  0
	isr_message_94:  db "testing for 94",  0
	isr_message_95:  db "testing for 95",  0
	isr_message_96:  db "testing for 96",  0
	isr_message_97:  db "testing for 97",  0
	isr_message_98:  db "testing for 98",  0
	isr_message_99:  db "testing for 99",  0
	isr_message_100: db "testing for 100", 0
	isr_message_101: db "testing for 101", 0
	isr_message_102: db "testing for 102", 0
	isr_message_103: db "testing for 103", 0
	isr_message_104: db "testing for 104", 0
	isr_message_105: db "testing for 105", 0
	isr_message_106: db "testing for 106", 0
	isr_message_107: db "testing for 107", 0
	isr_message_108: db "testing for 108", 0
	isr_message_109: db "testing for 109", 0
	isr_message_110: db "testing for 110", 0
	isr_message_111: db "testing for 111", 0
	isr_message_112: db "testing for 112", 0
	isr_message_113: db "testing for 113", 0
	isr_message_114: db "testing for 114", 0
	isr_message_115: db "testing for 115", 0
	isr_message_116: db "testing for 116", 0
	isr_message_117: db "testing for 117", 0
	isr_message_118: db "testing for 118", 0
	isr_message_119: db "testing for 119", 0
	isr_message_120: db "testing for 120", 0
	isr_message_121: db "testing for 121", 0
	isr_message_122: db "testing for 122", 0
	isr_message_123: db "testing for 123", 0
	isr_message_124: db "testing for 124", 0
	isr_message_125: db "testing for 125", 0
	isr_message_126: db "testing for 126", 0
	isr_message_127: db "testing for 127", 0
	isr_message_128: db "testing for 128", 0
	isr_message_129: db "testing for 129", 0
	isr_message_130: db "testing for 130", 0
	isr_message_131: db "testing for 131", 0
	isr_message_132: db "testing for 132", 0
	isr_message_133: db "testing for 133", 0
	isr_message_134: db "testing for 134", 0
	isr_message_135: db "testing for 135", 0
	isr_message_136: db "testing for 136", 0
	isr_message_137: db "testing for 137", 0
	isr_message_138: db "testing for 138", 0
	isr_message_139: db "testing for 139", 0
	isr_message_140: db "testing for 140", 0
	isr_message_141: db "testing for 141", 0
	isr_message_142: db "testing for 142", 0
	isr_message_143: db "testing for 143", 0
	isr_message_144: db "testing for 144", 0
	isr_message_145: db "testing for 145", 0
	isr_message_146: db "testing for 146", 0
	isr_message_147: db "testing for 147", 0
	isr_message_148: db "testing for 148", 0
	isr_message_149: db "testing for 149", 0
	isr_message_150: db "testing for 150", 0
	isr_message_151: db "testing for 151", 0
	isr_message_152: db "testing for 152", 0
	isr_message_153: db "testing for 153", 0
	isr_message_154: db "testing for 154", 0
	isr_message_155: db "testing for 155", 0
	isr_message_156: db "testing for 156", 0
	isr_message_157: db "testing for 157", 0
	isr_message_158: db "testing for 158", 0
	isr_message_159: db "testing for 159", 0
	isr_message_160: db "testing for 160", 0
	isr_message_161: db "testing for 161", 0
	isr_message_162: db "testing for 162", 0
	isr_message_163: db "testing for 163", 0
	isr_message_164: db "testing for 164", 0
	isr_message_165: db "testing for 165", 0
	isr_message_166: db "testing for 166", 0
	isr_message_167: db "testing for 167", 0
	isr_message_168: db "testing for 168", 0
	isr_message_169: db "testing for 169", 0
	isr_message_170: db "testing for 170", 0
	isr_message_171: db "testing for 171", 0
	isr_message_172: db "testing for 172", 0
	isr_message_173: db "testing for 173", 0
	isr_message_174: db "testing for 174", 0
	isr_message_175: db "testing for 175", 0
	isr_message_176: db "testing for 176", 0
	isr_message_177: db "testing for 177", 0
	isr_message_178: db "testing for 178", 0
	isr_message_179: db "testing for 179", 0
	isr_message_180: db "testing for 180", 0
	isr_message_181: db "testing for 181", 0
	isr_message_182: db "testing for 182", 0
	isr_message_183: db "testing for 183", 0
	isr_message_184: db "testing for 184", 0
	isr_message_185: db "testing for 185", 0
	isr_message_186: db "testing for 186", 0
	isr_message_187: db "testing for 187", 0
	isr_message_188: db "testing for 188", 0
	isr_message_189: db "testing for 189", 0
	isr_message_190: db "testing for 190", 0
	isr_message_191: db "testing for 191", 0
	isr_message_192: db "testing for 192", 0
	isr_message_193: db "testing for 193", 0
	isr_message_194: db "testing for 194", 0
	isr_message_195: db "testing for 195", 0
	isr_message_196: db "testing for 196", 0
	isr_message_197: db "testing for 197", 0
	isr_message_198: db "testing for 198", 0
	isr_message_199: db "testing for 199", 0
	isr_message_200: db "testing for 200", 0
	isr_message_201: db "testing for 201", 0
	isr_message_202: db "testing for 202", 0
	isr_message_203: db "testing for 203", 0
	isr_message_204: db "testing for 204", 0
	isr_message_205: db "testing for 205", 0
	isr_message_206: db "testing for 206", 0
	isr_message_207: db "testing for 207", 0
	isr_message_208: db "testing for 208", 0
	isr_message_209: db "testing for 209", 0
	isr_message_210: db "testing for 210", 0
	isr_message_211: db "testing for 211", 0
	isr_message_212: db "testing for 212", 0
	isr_message_213: db "testing for 213", 0
	isr_message_214: db "testing for 214", 0
	isr_message_215: db "testing for 215", 0
	isr_message_216: db "testing for 216", 0
	isr_message_217: db "testing for 217", 0
	isr_message_218: db "testing for 218", 0
	isr_message_219: db "testing for 219", 0
	isr_message_220: db "testing for 220", 0
	isr_message_221: db "testing for 221", 0
	isr_message_222: db "testing for 222", 0
	isr_message_223: db "testing for 223", 0
	isr_message_224: db "testing for 224", 0
	isr_message_225: db "testing for 225", 0
	isr_message_226: db "testing for 226", 0
	isr_message_227: db "testing for 227", 0
	isr_message_228: db "testing for 228", 0
	isr_message_229: db "testing for 229", 0
	isr_message_230: db "testing for 230", 0
	isr_message_231: db "testing for 231", 0
	isr_message_232: db "testing for 232", 0
	isr_message_233: db "testing for 233", 0
	isr_message_234: db "testing for 234", 0
	isr_message_235: db "testing for 235", 0
	isr_message_236: db "testing for 236", 0
	isr_message_237: db "testing for 237", 0
	isr_message_238: db "testing for 238", 0
	isr_message_239: db "testing for 239", 0
	isr_message_240: db "testing for 240", 0
	isr_message_241: db "testing for 241", 0
	isr_message_242: db "testing for 242", 0
	isr_message_243: db "testing for 243", 0
	isr_message_244: db "testing for 244", 0
	isr_message_245: db "testing for 245", 0
	isr_message_246: db "testing for 246", 0
	isr_message_247: db "testing for 247", 0
	isr_message_248: db "testing for 248", 0
	isr_message_249: db "testing for 249", 0
	isr_message_250: db "testing for 250", 0
	isr_message_251: db "testing for 251", 0
	isr_message_252: db "testing for 252", 0
	isr_message_253: db "testing for 253", 0
	isr_message_254: db "testing for 254", 0
	isr_message_255: db "testing for 255", 0

	isr_messages: dd isr_message_0, isr_message_1, isr_message_2, isr_message_3, isr_message_4, \
		isr_message_5, isr_message_6, isr_message_7, isr_message_8, isr_message_9, isr_message_10, \
		isr_message_11, isr_message_12, isr_message_13, isr_message_14, isr_message_15, isr_message_16, \
		isr_message_17, isr_message_18, isr_message_19, isr_message_20, isr_message_21, isr_message_22, \
		isr_message_23, isr_message_24, isr_message_25, isr_message_26, isr_message_27, isr_message_28, \
		isr_message_29, isr_message_30, isr_message_31, isr_message_32, isr_message_33, isr_message_34, \
		isr_message_35, isr_message_36, isr_message_37, isr_message_38, isr_message_39, isr_message_40, \
		isr_message_41, isr_message_42, isr_message_43, isr_message_44, isr_message_45, isr_message_46, \
		isr_message_47, isr_message_48, isr_message_49, isr_message_50, isr_message_51, isr_message_52, \
		isr_message_53, isr_message_54, isr_message_55, isr_message_56, isr_message_57, isr_message_58, \
		isr_message_59, isr_message_60, isr_message_61, isr_message_62, isr_message_63, isr_message_64, \
		isr_message_65, isr_message_66, isr_message_67, isr_message_68, isr_message_69, isr_message_70, \
		isr_message_71, isr_message_72, isr_message_73, isr_message_74, isr_message_75, isr_message_76, \
		isr_message_77, isr_message_78, isr_message_79, isr_message_80, isr_message_81, isr_message_82, \
		isr_message_83, isr_message_84, isr_message_85, isr_message_86, isr_message_87, isr_message_88, \
		isr_message_89, isr_message_90, isr_message_91, isr_message_92, isr_message_93, isr_message_94, \
		isr_message_95, isr_message_96, isr_message_97, isr_message_98, isr_message_99, isr_message_100, \
		isr_message_101, isr_message_102, isr_message_103, isr_message_104, isr_message_105, isr_message_106, \
		isr_message_107, isr_message_108, isr_message_109, isr_message_110, isr_message_111, isr_message_112, \
		isr_message_113, isr_message_114, isr_message_115, isr_message_116, isr_message_117, isr_message_118, \
		isr_message_119, isr_message_120, isr_message_121, isr_message_122, isr_message_123, isr_message_124, \
		isr_message_125, isr_message_126, isr_message_127, isr_message_128, isr_message_129, isr_message_130, \
		isr_message_131, isr_message_132, isr_message_133, isr_message_134, isr_message_135, isr_message_136, \
		isr_message_137, isr_message_138, isr_message_139, isr_message_140, isr_message_141, isr_message_142, \
		isr_message_143, isr_message_144, isr_message_145, isr_message_146, isr_message_147, isr_message_148, \
		isr_message_149, isr_message_150, isr_message_151, isr_message_152, isr_message_153, isr_message_154, \
		isr_message_155, isr_message_156, isr_message_157, isr_message_158, isr_message_159, isr_message_160, \
		isr_message_161, isr_message_162, isr_message_163, isr_message_164, isr_message_165, isr_message_166, \
		isr_message_167, isr_message_168, isr_message_169, isr_message_170, isr_message_171, isr_message_172, \
		isr_message_173, isr_message_174, isr_message_175, isr_message_176, isr_message_177, isr_message_178, \
		isr_message_179, isr_message_180, isr_message_181, isr_message_182, isr_message_183, isr_message_184, \
		isr_message_185, isr_message_186, isr_message_187, isr_message_188, isr_message_189, isr_message_190, \
		isr_message_191, isr_message_192, isr_message_193, isr_message_194, isr_message_195, isr_message_196, \
		isr_message_197, isr_message_198, isr_message_199, isr_message_200, isr_message_201, isr_message_202, \
		isr_message_203, isr_message_204, isr_message_205, isr_message_206, isr_message_207, isr_message_208, \
		isr_message_209, isr_message_210, isr_message_211, isr_message_212, isr_message_213, isr_message_214, \
		isr_message_215, isr_message_216, isr_message_217, isr_message_218, isr_message_219, isr_message_220, \
		isr_message_221, isr_message_222, isr_message_223, isr_message_224, isr_message_225, isr_message_226, \
		isr_message_227, isr_message_228, isr_message_229, isr_message_230, isr_message_231, isr_message_232, \
		isr_message_233, isr_message_234, isr_message_235, isr_message_236, isr_message_237, isr_message_238, \
		isr_message_239, isr_message_240, isr_message_241, isr_message_242, isr_message_243, isr_message_244, \
		isr_message_245, isr_message_246, isr_message_247, isr_message_248, isr_message_249, isr_message_250, \
		isr_message_251, isr_message_252, isr_message_253, isr_message_254, isr_message_255, \

section .bss
	idt: resq idt_length

