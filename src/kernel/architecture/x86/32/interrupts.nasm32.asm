[bits 32]
extern printf

section .text

; -------------------- IRQs --------------------

global IRQ_enable
IRQ_enable:
	sti
	ret

global IRQ_disable
IRQ_disable:
	cli
	ret

; -------------------- ISRs --------------------

ISR_load_kernel_data_segment:
	mov eax, 16 ; kernel data segment selector in GDT
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	ret

ISR_end:
	; restore segments
	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	popa
	iret

%macro ISR 1
	global ISR%1
	ISR%1:
		pusha
		xor eax, eax
		mov ax, ds
		push eax
		call ISR_load_kernel_data_segment

		push dword [ISR_messages + (4 * %1)]
		push dword %1
		push dword ISR_message_intro
		call printf
		add esp, 12

		jmp ISR_end
%endmacro

%assign i 0
%rep 256
	ISR i
	%assign i i+1
%endrep

; -------------------- GDT --------------------

global GDT_load_x86
GDT_load_x86:
	push ebp
	mov ebp, esp

	; load GDT
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

%define IDT_gate_flags_type_task            0b0_00_0_0101 ; note that in this case, the ISR_address is unused and should be set to zero.
%define IDT_gate_flags_type16_bit_interrupt 0b0_00_0_0110
%define IDT_gate_flags_type16_bit_trap      0b0_00_0_0111
%define IDT_gate_flags_type32_bit_interrupt 0b0_00_0_1110
%define IDT_gate_flags_type32_bit_trap      0b0_00_0_1111
%define IDT_gate_flags_ring0		        0b0_00_0_0000
%define IDT_gate_flags_ring3		        0b0_11_0_0000
%define IDT_gate_flags_present		  0b1_00_0_0000

; params: index of gate [0-255], IDT_gate_flags byte
IDT_gate_add:
	; eax: offset in IDT, edx: ISR address, ecx: IDT_gate flags
	mov eax, [esp + 4]
	shl eax, 2		; eax = offset in ISRs
	mov edx, [ISRs + eax]
	mov ecx, [esp + 8]
	shl eax, 1 		; eax = offset in IDT

	; --- IDT_gate ---
	mov word [IDT + eax], dx                  ; ISR address low
	mov word [IDT + eax + 2], 8 		  ; kernel code segment selector
	mov byte [IDT + eax + 4], 0               ; reserved byte
	mov byte [IDT + eax + 5], cl              ; flags byte
	; ISR address high
	shr edx, 16
	mov word [IDT + eax + 6], dx

	ret


global IDT_load_x86
IDT_load_x86:
	push ebp
	mov ebp, esp
	push ebx

	xor ebx, ebx
	push dword (IDT_gate_flags_type32_bit_trap | IDT_gate_flags_ring0 | IDT_gate_flags_present)
	.for_exceptions:
		cmp ebx, 31
		jg .end_for_exceptions

		push ebx
		call IDT_gate_add
		add esp, 4

		inc ebx
		jmp .for_exceptions
	.end_for_exceptions:
	add esp, 4

	push dword (IDT_gate_flags_type32_bit_interrupt | IDT_gate_flags_ring0 | IDT_gate_flags_present)
	.for_interrupts:
		cmp ebx, IDT_length
		jge .end_for_interrupts

		push ebx
		call IDT_gate_add
		add esp, 4

		inc ebx
		jmp .for_interrupts
	.end_for_interrupts:
	add esp, 4

	lidt [IDT_descriptor]

	pop ebx
	mov esp, ebp
	pop ebp
	ret

section .rodata
	ISR_message_intro: db "interrupt %i: %s",10,0

	IDT_length equ 256

	IDT_descriptor: dw (IDT_length * 8) - 1 ; sizeof(IDT) - 1
		       dd IDT

	ISRs: dd ISR0, ISR1, ISR2, ISR3, ISR4, ISR5, ISR6, ISR7, ISR8, ISR9, ISR10, ISR11, ISR12, ISR13, ISR14, \
		ISR15, ISR16, ISR17, ISR18, ISR19, ISR20, ISR21, ISR22, ISR23, ISR24, ISR25, ISR26, ISR27, \
		ISR28, ISR29, ISR30, ISR31, ISR32, ISR33, ISR34, ISR35, ISR36, ISR37, ISR38, ISR39, ISR40, \
		ISR41, ISR42, ISR43, ISR44, ISR45, ISR46, ISR47, ISR48, ISR49, ISR50, ISR51, ISR52, ISR53, \
		ISR54, ISR55, ISR56, ISR57, ISR58, ISR59, ISR60, ISR61, ISR62, ISR63, ISR64, ISR65, ISR66, \
		ISR67, ISR68, ISR69, ISR70, ISR71, ISR72, ISR73, ISR74, ISR75, ISR76, ISR77, ISR78, ISR79, \
		ISR80, ISR81, ISR82, ISR83, ISR84, ISR85, ISR86, ISR87, ISR88, ISR89, ISR90, ISR91, ISR92, \
		ISR93, ISR94, ISR95, ISR96, ISR97, ISR98, ISR99, ISR100, ISR101, ISR102, ISR103, ISR104, ISR105, \
		ISR106, ISR107, ISR108, ISR109, ISR110, ISR111, ISR112, ISR113, ISR114, ISR115, ISR116, ISR117, ISR118, \
		ISR119, ISR120, ISR121, ISR122, ISR123, ISR124, ISR125, ISR126, ISR127, ISR128, ISR129, ISR130, ISR131, \
		ISR132, ISR133, ISR134, ISR135, ISR136, ISR137, ISR138, ISR139, ISR140, ISR141, ISR142, ISR143, ISR144, \
		ISR145, ISR146, ISR147, ISR148, ISR149, ISR150, ISR151, ISR152, ISR153, ISR154, ISR155, ISR156, ISR157, \
		ISR158, ISR159, ISR160, ISR161, ISR162, ISR163, ISR164, ISR165, ISR166, ISR167, ISR168, ISR169, ISR170, \
		ISR171, ISR172, ISR173, ISR174, ISR175, ISR176, ISR177, ISR178, ISR179, ISR180, ISR181, ISR182, ISR183, \
		ISR184, ISR185, ISR186, ISR187, ISR188, ISR189, ISR190, ISR191, ISR192, ISR193, ISR194, ISR195, ISR196, \
		ISR197, ISR198, ISR199, ISR200, ISR201, ISR202, ISR203, ISR204, ISR205, ISR206, ISR207, ISR208, ISR209, \
		ISR210, ISR211, ISR212, ISR213, ISR214, ISR215, ISR216, ISR217, ISR218, ISR219, ISR220, ISR221, ISR222, \
		ISR223, ISR224, ISR225, ISR226, ISR227, ISR228, ISR229, ISR230, ISR231, ISR232, ISR233, ISR234, ISR235, \
		ISR236, ISR237, ISR238, ISR239, ISR240, ISR241, ISR242, ISR243, ISR244, ISR245, ISR246, ISR247, ISR248, \
		ISR249, ISR250, ISR251, ISR252, ISR253, ISR254, ISR255

	ISR_message_0:   db "Exception 0: Division by 0", 0
	ISR_message_1:   db "Exception 1: Debug", 0
	ISR_message_2:   db "Exception 2: Non-maskable interrupt", 0
	ISR_message_3:   db "Exception 3: Breakpoint", 0
	ISR_message_4:   db "Exception 4: Overflow", 0
	ISR_message_5:   db "Exception 5: Bound range exceeded", 0
	ISR_message_6:   db "Exception 6: Invalid opcode", 0
	ISR_message_7:   db "Exception 7: Device not available", 0
	ISR_message_8:   db "Exception 8: Double fault", 0
	ISR_message_9:   db "Exception 9: Coprocessor segment overrun", 0
	ISR_message_10:  db "Exception 10: Invalid TSS", 0
	ISR_message_11:  db "Exception 11: Segment not present", 0
	ISR_message_12:  db "Exception 12: Stack segment fault", 0
	ISR_message_13:  db "Exception 13: General protection fault", 0
	ISR_message_14:  db "Exception 14: Page fault", 0
	ISR_message_15:  db "Exception 15: Reserved", 0
	ISR_message_16:  db "Exception 16: x87 floating point exception", 0
	ISR_message_17:  db "Exception 17: Alignment check", 0
	ISR_message_18:  db "Exception 18: Machine check", 0
	ISR_message_19:  db "Exception 19: SIMD floating point exception", 0
	ISR_message_20:  db "Exception 20: Virtualization exception", 0
	ISR_message_21:  db "Exception 21: Control protection exception", 0
	ISR_message_22:  db "Exception 22: Reserved", 0
	ISR_message_23:  db "Exception 23: Reserved", 0
	ISR_message_24:  db "Exception 24: Reserved", 0
	ISR_message_25:  db "Exception 25: Reserved", 0
	ISR_message_26:  db "Exception 26: Reserved", 0
	ISR_message_27:  db "Exception 27: Reserved", 0
	ISR_message_28:  db "Exception 28: Hypervisor injection exception", 0
	ISR_message_29:  db "Exception 29: VMM communication exception", 0
	ISR_message_30:  db "Exception 30: Security exception", 0
	ISR_message_31:  db "Exception 31: Reserved", 0
	ISR_message_32:  db "Unhandeled IRQ 32",  0
	ISR_message_33:  db "Unhandeled IRQ 33",  0
	ISR_message_34:  db "Unhandeled IRQ 34",  0
	ISR_message_35:  db "Unhandeled IRQ 35",  0
	ISR_message_36:  db "Unhandeled IRQ 36",  0
	ISR_message_37:  db "Unhandeled IRQ 37",  0
	ISR_message_38:  db "Unhandeled IRQ 38",  0
	ISR_message_39:  db "Unhandeled IRQ 39",  0
	ISR_message_40:  db "Unhandeled IRQ 40",  0
	ISR_message_41:  db "Unhandeled IRQ 41",  0
	ISR_message_42:  db "Unhandeled IRQ 42",  0
	ISR_message_43:  db "Unhandeled IRQ 43",  0
	ISR_message_44:  db "Unhandeled IRQ 44",  0
	ISR_message_45:  db "Unhandeled IRQ 45",  0
	ISR_message_46:  db "Unhandeled IRQ 46",  0
	ISR_message_47:  db "Unhandeled IRQ 47",  0
	ISR_message_48:  db "Unhandeled IRQ 48",  0
	ISR_message_49:  db "Unhandeled IRQ 49",  0
	ISR_message_50:  db "Unhandeled IRQ 50",  0
	ISR_message_51:  db "Unhandeled IRQ 51",  0
	ISR_message_52:  db "Unhandeled IRQ 52",  0
	ISR_message_53:  db "Unhandeled IRQ 53",  0
	ISR_message_54:  db "Unhandeled IRQ 54",  0
	ISR_message_55:  db "Unhandeled IRQ 55",  0
	ISR_message_56:  db "Unhandeled IRQ 56",  0
	ISR_message_57:  db "Unhandeled IRQ 57",  0
	ISR_message_58:  db "Unhandeled IRQ 58",  0
	ISR_message_59:  db "Unhandeled IRQ 59",  0
	ISR_message_60:  db "Unhandeled IRQ 60",  0
	ISR_message_61:  db "Unhandeled IRQ 61",  0
	ISR_message_62:  db "Unhandeled IRQ 62",  0
	ISR_message_63:  db "Unhandeled IRQ 63",  0
	ISR_message_64:  db "Unhandeled IRQ 64",  0
	ISR_message_65:  db "Unhandeled IRQ 65",  0
	ISR_message_66:  db "Unhandeled IRQ 66",  0
	ISR_message_67:  db "Unhandeled IRQ 67",  0
	ISR_message_68:  db "Unhandeled IRQ 68",  0
	ISR_message_69:  db "Unhandeled IRQ 69",  0
	ISR_message_70:  db "Unhandeled IRQ 70",  0
	ISR_message_71:  db "Unhandeled IRQ 71",  0
	ISR_message_72:  db "Unhandeled IRQ 72",  0
	ISR_message_73:  db "Unhandeled IRQ 73",  0
	ISR_message_74:  db "Unhandeled IRQ 74",  0
	ISR_message_75:  db "Unhandeled IRQ 75",  0
	ISR_message_76:  db "Unhandeled IRQ 76",  0
	ISR_message_77:  db "Unhandeled IRQ 77",  0
	ISR_message_78:  db "Unhandeled IRQ 78",  0
	ISR_message_79:  db "Unhandeled IRQ 79",  0
	ISR_message_80:  db "Unhandeled IRQ 80",  0
	ISR_message_81:  db "Unhandeled IRQ 81",  0
	ISR_message_82:  db "Unhandeled IRQ 82",  0
	ISR_message_83:  db "Unhandeled IRQ 83",  0
	ISR_message_84:  db "Unhandeled IRQ 84",  0
	ISR_message_85:  db "Unhandeled IRQ 85",  0
	ISR_message_86:  db "Unhandeled IRQ 86",  0
	ISR_message_87:  db "Unhandeled IRQ 87",  0
	ISR_message_88:  db "Unhandeled IRQ 88",  0
	ISR_message_89:  db "Unhandeled IRQ 89",  0
	ISR_message_90:  db "Unhandeled IRQ 90",  0
	ISR_message_91:  db "Unhandeled IRQ 91",  0
	ISR_message_92:  db "Unhandeled IRQ 92",  0
	ISR_message_93:  db "Unhandeled IRQ 93",  0
	ISR_message_94:  db "Unhandeled IRQ 94",  0
	ISR_message_95:  db "Unhandeled IRQ 95",  0
	ISR_message_96:  db "Unhandeled IRQ 96",  0
	ISR_message_97:  db "Unhandeled IRQ 97",  0
	ISR_message_98:  db "Unhandeled IRQ 98",  0
	ISR_message_99:  db "Unhandeled IRQ 99",  0
	ISR_message_100: db "Unhandeled IRQ 100", 0
	ISR_message_101: db "Unhandeled IRQ 101", 0
	ISR_message_102: db "Unhandeled IRQ 102", 0
	ISR_message_103: db "Unhandeled IRQ 103", 0
	ISR_message_104: db "Unhandeled IRQ 104", 0
	ISR_message_105: db "Unhandeled IRQ 105", 0
	ISR_message_106: db "Unhandeled IRQ 106", 0
	ISR_message_107: db "Unhandeled IRQ 107", 0
	ISR_message_108: db "Unhandeled IRQ 108", 0
	ISR_message_109: db "Unhandeled IRQ 109", 0
	ISR_message_110: db "Unhandeled IRQ 110", 0
	ISR_message_111: db "Unhandeled IRQ 111", 0
	ISR_message_112: db "Unhandeled IRQ 112", 0
	ISR_message_113: db "Unhandeled IRQ 113", 0
	ISR_message_114: db "Unhandeled IRQ 114", 0
	ISR_message_115: db "Unhandeled IRQ 115", 0
	ISR_message_116: db "Unhandeled IRQ 116", 0
	ISR_message_117: db "Unhandeled IRQ 117", 0
	ISR_message_118: db "Unhandeled IRQ 118", 0
	ISR_message_119: db "Unhandeled IRQ 119", 0
	ISR_message_120: db "Unhandeled IRQ 120", 0
	ISR_message_121: db "Unhandeled IRQ 121", 0
	ISR_message_122: db "Unhandeled IRQ 122", 0
	ISR_message_123: db "Unhandeled IRQ 123", 0
	ISR_message_124: db "Unhandeled IRQ 124", 0
	ISR_message_125: db "Unhandeled IRQ 125", 0
	ISR_message_126: db "Unhandeled IRQ 126", 0
	ISR_message_127: db "Unhandeled IRQ 127", 0
	ISR_message_128: db "Unhandeled IRQ 128", 0
	ISR_message_129: db "Unhandeled IRQ 129", 0
	ISR_message_130: db "Unhandeled IRQ 130", 0
	ISR_message_131: db "Unhandeled IRQ 131", 0
	ISR_message_132: db "Unhandeled IRQ 132", 0
	ISR_message_133: db "Unhandeled IRQ 133", 0
	ISR_message_134: db "Unhandeled IRQ 134", 0
	ISR_message_135: db "Unhandeled IRQ 135", 0
	ISR_message_136: db "Unhandeled IRQ 136", 0
	ISR_message_137: db "Unhandeled IRQ 137", 0
	ISR_message_138: db "Unhandeled IRQ 138", 0
	ISR_message_139: db "Unhandeled IRQ 139", 0
	ISR_message_140: db "Unhandeled IRQ 140", 0
	ISR_message_141: db "Unhandeled IRQ 141", 0
	ISR_message_142: db "Unhandeled IRQ 142", 0
	ISR_message_143: db "Unhandeled IRQ 143", 0
	ISR_message_144: db "Unhandeled IRQ 144", 0
	ISR_message_145: db "Unhandeled IRQ 145", 0
	ISR_message_146: db "Unhandeled IRQ 146", 0
	ISR_message_147: db "Unhandeled IRQ 147", 0
	ISR_message_148: db "Unhandeled IRQ 148", 0
	ISR_message_149: db "Unhandeled IRQ 149", 0
	ISR_message_150: db "Unhandeled IRQ 150", 0
	ISR_message_151: db "Unhandeled IRQ 151", 0
	ISR_message_152: db "Unhandeled IRQ 152", 0
	ISR_message_153: db "Unhandeled IRQ 153", 0
	ISR_message_154: db "Unhandeled IRQ 154", 0
	ISR_message_155: db "Unhandeled IRQ 155", 0
	ISR_message_156: db "Unhandeled IRQ 156", 0
	ISR_message_157: db "Unhandeled IRQ 157", 0
	ISR_message_158: db "Unhandeled IRQ 158", 0
	ISR_message_159: db "Unhandeled IRQ 159", 0
	ISR_message_160: db "Unhandeled IRQ 160", 0
	ISR_message_161: db "Unhandeled IRQ 161", 0
	ISR_message_162: db "Unhandeled IRQ 162", 0
	ISR_message_163: db "Unhandeled IRQ 163", 0
	ISR_message_164: db "Unhandeled IRQ 164", 0
	ISR_message_165: db "Unhandeled IRQ 165", 0
	ISR_message_166: db "Unhandeled IRQ 166", 0
	ISR_message_167: db "Unhandeled IRQ 167", 0
	ISR_message_168: db "Unhandeled IRQ 168", 0
	ISR_message_169: db "Unhandeled IRQ 169", 0
	ISR_message_170: db "Unhandeled IRQ 170", 0
	ISR_message_171: db "Unhandeled IRQ 171", 0
	ISR_message_172: db "Unhandeled IRQ 172", 0
	ISR_message_173: db "Unhandeled IRQ 173", 0
	ISR_message_174: db "Unhandeled IRQ 174", 0
	ISR_message_175: db "Unhandeled IRQ 175", 0
	ISR_message_176: db "Unhandeled IRQ 176", 0
	ISR_message_177: db "Unhandeled IRQ 177", 0
	ISR_message_178: db "Unhandeled IRQ 178", 0
	ISR_message_179: db "Unhandeled IRQ 179", 0
	ISR_message_180: db "Unhandeled IRQ 180", 0
	ISR_message_181: db "Unhandeled IRQ 181", 0
	ISR_message_182: db "Unhandeled IRQ 182", 0
	ISR_message_183: db "Unhandeled IRQ 183", 0
	ISR_message_184: db "Unhandeled IRQ 184", 0
	ISR_message_185: db "Unhandeled IRQ 185", 0
	ISR_message_186: db "Unhandeled IRQ 186", 0
	ISR_message_187: db "Unhandeled IRQ 187", 0
	ISR_message_188: db "Unhandeled IRQ 188", 0
	ISR_message_189: db "Unhandeled IRQ 189", 0
	ISR_message_190: db "Unhandeled IRQ 190", 0
	ISR_message_191: db "Unhandeled IRQ 191", 0
	ISR_message_192: db "Unhandeled IRQ 192", 0
	ISR_message_193: db "Unhandeled IRQ 193", 0
	ISR_message_194: db "Unhandeled IRQ 194", 0
	ISR_message_195: db "Unhandeled IRQ 195", 0
	ISR_message_196: db "Unhandeled IRQ 196", 0
	ISR_message_197: db "Unhandeled IRQ 197", 0
	ISR_message_198: db "Unhandeled IRQ 198", 0
	ISR_message_199: db "Unhandeled IRQ 199", 0
	ISR_message_200: db "Unhandeled IRQ 200", 0
	ISR_message_201: db "Unhandeled IRQ 201", 0
	ISR_message_202: db "Unhandeled IRQ 202", 0
	ISR_message_203: db "Unhandeled IRQ 203", 0
	ISR_message_204: db "Unhandeled IRQ 204", 0
	ISR_message_205: db "Unhandeled IRQ 205", 0
	ISR_message_206: db "Unhandeled IRQ 206", 0
	ISR_message_207: db "Unhandeled IRQ 207", 0
	ISR_message_208: db "Unhandeled IRQ 208", 0
	ISR_message_209: db "Unhandeled IRQ 209", 0
	ISR_message_210: db "Unhandeled IRQ 210", 0
	ISR_message_211: db "Unhandeled IRQ 211", 0
	ISR_message_212: db "Unhandeled IRQ 212", 0
	ISR_message_213: db "Unhandeled IRQ 213", 0
	ISR_message_214: db "Unhandeled IRQ 214", 0
	ISR_message_215: db "Unhandeled IRQ 215", 0
	ISR_message_216: db "Unhandeled IRQ 216", 0
	ISR_message_217: db "Unhandeled IRQ 217", 0
	ISR_message_218: db "Unhandeled IRQ 218", 0
	ISR_message_219: db "Unhandeled IRQ 219", 0
	ISR_message_220: db "Unhandeled IRQ 220", 0
	ISR_message_221: db "Unhandeled IRQ 221", 0
	ISR_message_222: db "Unhandeled IRQ 222", 0
	ISR_message_223: db "Unhandeled IRQ 223", 0
	ISR_message_224: db "Unhandeled IRQ 224", 0
	ISR_message_225: db "Unhandeled IRQ 225", 0
	ISR_message_226: db "Unhandeled IRQ 226", 0
	ISR_message_227: db "Unhandeled IRQ 227", 0
	ISR_message_228: db "Unhandeled IRQ 228", 0
	ISR_message_229: db "Unhandeled IRQ 229", 0
	ISR_message_230: db "Unhandeled IRQ 230", 0
	ISR_message_231: db "Unhandeled IRQ 231", 0
	ISR_message_232: db "Unhandeled IRQ 232", 0
	ISR_message_233: db "Unhandeled IRQ 233", 0
	ISR_message_234: db "Unhandeled IRQ 234", 0
	ISR_message_235: db "Unhandeled IRQ 235", 0
	ISR_message_236: db "Unhandeled IRQ 236", 0
	ISR_message_237: db "Unhandeled IRQ 237", 0
	ISR_message_238: db "Unhandeled IRQ 238", 0
	ISR_message_239: db "Unhandeled IRQ 239", 0
	ISR_message_240: db "Unhandeled IRQ 240", 0
	ISR_message_241: db "Unhandeled IRQ 241", 0
	ISR_message_242: db "Unhandeled IRQ 242", 0
	ISR_message_243: db "Unhandeled IRQ 243", 0
	ISR_message_244: db "Unhandeled IRQ 244", 0
	ISR_message_245: db "Unhandeled IRQ 245", 0
	ISR_message_246: db "Unhandeled IRQ 246", 0
	ISR_message_247: db "Unhandeled IRQ 247", 0
	ISR_message_248: db "Unhandeled IRQ 248", 0
	ISR_message_249: db "Unhandeled IRQ 249", 0
	ISR_message_250: db "Unhandeled IRQ 250", 0
	ISR_message_251: db "Unhandeled IRQ 251", 0
	ISR_message_252: db "Unhandeled IRQ 252", 0
	ISR_message_253: db "Unhandeled IRQ 253", 0
	ISR_message_254: db "Unhandeled IRQ 254", 0
	ISR_message_255: db "Unhandeled IRQ 255", 0

	ISR_messages: dd ISR_message_0, ISR_message_1, ISR_message_2, ISR_message_3, ISR_message_4, \
		ISR_message_5, ISR_message_6, ISR_message_7, ISR_message_8, ISR_message_9, ISR_message_10, \
		ISR_message_11, ISR_message_12, ISR_message_13, ISR_message_14, ISR_message_15, ISR_message_16, \
		ISR_message_17, ISR_message_18, ISR_message_19, ISR_message_20, ISR_message_21, ISR_message_22, \
		ISR_message_23, ISR_message_24, ISR_message_25, ISR_message_26, ISR_message_27, ISR_message_28, \
		ISR_message_29, ISR_message_30, ISR_message_31, ISR_message_32, ISR_message_33, ISR_message_34, \
		ISR_message_35, ISR_message_36, ISR_message_37, ISR_message_38, ISR_message_39, ISR_message_40, \
		ISR_message_41, ISR_message_42, ISR_message_43, ISR_message_44, ISR_message_45, ISR_message_46, \
		ISR_message_47, ISR_message_48, ISR_message_49, ISR_message_50, ISR_message_51, ISR_message_52, \
		ISR_message_53, ISR_message_54, ISR_message_55, ISR_message_56, ISR_message_57, ISR_message_58, \
		ISR_message_59, ISR_message_60, ISR_message_61, ISR_message_62, ISR_message_63, ISR_message_64, \
		ISR_message_65, ISR_message_66, ISR_message_67, ISR_message_68, ISR_message_69, ISR_message_70, \
		ISR_message_71, ISR_message_72, ISR_message_73, ISR_message_74, ISR_message_75, ISR_message_76, \
		ISR_message_77, ISR_message_78, ISR_message_79, ISR_message_80, ISR_message_81, ISR_message_82, \
		ISR_message_83, ISR_message_84, ISR_message_85, ISR_message_86, ISR_message_87, ISR_message_88, \
		ISR_message_89, ISR_message_90, ISR_message_91, ISR_message_92, ISR_message_93, ISR_message_94, \
		ISR_message_95, ISR_message_96, ISR_message_97, ISR_message_98, ISR_message_99, ISR_message_100, \
		ISR_message_101, ISR_message_102, ISR_message_103, ISR_message_104, ISR_message_105, ISR_message_106, \
		ISR_message_107, ISR_message_108, ISR_message_109, ISR_message_110, ISR_message_111, ISR_message_112, \
		ISR_message_113, ISR_message_114, ISR_message_115, ISR_message_116, ISR_message_117, ISR_message_118, \
		ISR_message_119, ISR_message_120, ISR_message_121, ISR_message_122, ISR_message_123, ISR_message_124, \
		ISR_message_125, ISR_message_126, ISR_message_127, ISR_message_128, ISR_message_129, ISR_message_130, \
		ISR_message_131, ISR_message_132, ISR_message_133, ISR_message_134, ISR_message_135, ISR_message_136, \
		ISR_message_137, ISR_message_138, ISR_message_139, ISR_message_140, ISR_message_141, ISR_message_142, \
		ISR_message_143, ISR_message_144, ISR_message_145, ISR_message_146, ISR_message_147, ISR_message_148, \
		ISR_message_149, ISR_message_150, ISR_message_151, ISR_message_152, ISR_message_153, ISR_message_154, \
		ISR_message_155, ISR_message_156, ISR_message_157, ISR_message_158, ISR_message_159, ISR_message_160, \
		ISR_message_161, ISR_message_162, ISR_message_163, ISR_message_164, ISR_message_165, ISR_message_166, \
		ISR_message_167, ISR_message_168, ISR_message_169, ISR_message_170, ISR_message_171, ISR_message_172, \
		ISR_message_173, ISR_message_174, ISR_message_175, ISR_message_176, ISR_message_177, ISR_message_178, \
		ISR_message_179, ISR_message_180, ISR_message_181, ISR_message_182, ISR_message_183, ISR_message_184, \
		ISR_message_185, ISR_message_186, ISR_message_187, ISR_message_188, ISR_message_189, ISR_message_190, \
		ISR_message_191, ISR_message_192, ISR_message_193, ISR_message_194, ISR_message_195, ISR_message_196, \
		ISR_message_197, ISR_message_198, ISR_message_199, ISR_message_200, ISR_message_201, ISR_message_202, \
		ISR_message_203, ISR_message_204, ISR_message_205, ISR_message_206, ISR_message_207, ISR_message_208, \
		ISR_message_209, ISR_message_210, ISR_message_211, ISR_message_212, ISR_message_213, ISR_message_214, \
		ISR_message_215, ISR_message_216, ISR_message_217, ISR_message_218, ISR_message_219, ISR_message_220, \
		ISR_message_221, ISR_message_222, ISR_message_223, ISR_message_224, ISR_message_225, ISR_message_226, \
		ISR_message_227, ISR_message_228, ISR_message_229, ISR_message_230, ISR_message_231, ISR_message_232, \
		ISR_message_233, ISR_message_234, ISR_message_235, ISR_message_236, ISR_message_237, ISR_message_238, \
		ISR_message_239, ISR_message_240, ISR_message_241, ISR_message_242, ISR_message_243, ISR_message_244, \
		ISR_message_245, ISR_message_246, ISR_message_247, ISR_message_248, ISR_message_249, ISR_message_250, \
		ISR_message_251, ISR_message_252, ISR_message_253, ISR_message_254, ISR_message_255, \

section .bss
	IDT: resq IDT_length

