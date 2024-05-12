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
	mov eax, 16 ; kernel data segment selector in g_d_t
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

	ISR_message_0:   db "division by 0", 0
	ISR_message_1:   db "debug", 0
	ISR_message_2:   db "non-maskable interrupt", 0
	ISR_message_3:   db "breakpoint", 0
	ISR_message_4:   db "overflow", 0
	ISR_message_5:   db "bound range exceeded", 0
	ISR_message_6:   db "invalid opcode", 0
	ISR_message_7:   db "device not available", 0
	ISR_message_8:   db "double fault", 0
	ISR_message_9:   db "coprocessor segment overrun", 0
	ISR_message_10:  db "invalid t_s_s", 0
	ISR_message_11:  db "segment not present", 0
	ISR_message_12:  db "stack-_segment fault", 0
	ISR_message_13:  db "general protection fault", 0
	ISR_message_14:  db "page fault", 0
	ISR_message_15:  db "reserved", 0
	ISR_message_16:  db "x87 floating-_point exception", 0
	ISR_message_17:  db "alignment check", 0
	ISR_message_18:  db "machine check", 0
	ISR_message_19:  db "s_i_m_d floating-_point exception", 0
	ISR_message_20:  db "virtualization exception", 0
	ISR_message_21:  db "control protection exception", 0
	ISR_message_22:  db "reserved", 0
	ISR_message_23:  db "reserved", 0
	ISR_message_24:  db "reserved", 0
	ISR_message_25:  db "reserved", 0
	ISR_message_26:  db "reserved", 0
	ISR_message_27:  db "reserved", 0
	ISR_message_28:  db "hypervisor injection exception", 0
	ISR_message_29:  db "v_m_m communication exception", 0
	ISR_message_30:  db "security exception", 0
	ISR_message_31:  db "reserved", 0
	ISR_message_32:  db "testing for 32",  0
	ISR_message_33:  db "testing for 33",  0
	ISR_message_34:  db "testing for 34",  0
	ISR_message_35:  db "testing for 35",  0
	ISR_message_36:  db "testing for 36",  0
	ISR_message_37:  db "testing for 37",  0
	ISR_message_38:  db "testing for 38",  0
	ISR_message_39:  db "testing for 39",  0
	ISR_message_40:  db "testing for 40",  0
	ISR_message_41:  db "testing for 41",  0
	ISR_message_42:  db "testing for 42",  0
	ISR_message_43:  db "testing for 43",  0
	ISR_message_44:  db "testing for 44",  0
	ISR_message_45:  db "testing for 45",  0
	ISR_message_46:  db "testing for 46",  0
	ISR_message_47:  db "testing for 47",  0
	ISR_message_48:  db "testing for 48",  0
	ISR_message_49:  db "testing for 49",  0
	ISR_message_50:  db "testing for 50",  0
	ISR_message_51:  db "testing for 51",  0
	ISR_message_52:  db "testing for 52",  0
	ISR_message_53:  db "testing for 53",  0
	ISR_message_54:  db "testing for 54",  0
	ISR_message_55:  db "testing for 55",  0
	ISR_message_56:  db "testing for 56",  0
	ISR_message_57:  db "testing for 57",  0
	ISR_message_58:  db "testing for 58",  0
	ISR_message_59:  db "testing for 59",  0
	ISR_message_60:  db "testing for 60",  0
	ISR_message_61:  db "testing for 61",  0
	ISR_message_62:  db "testing for 62",  0
	ISR_message_63:  db "testing for 63",  0
	ISR_message_64:  db "testing for 64",  0
	ISR_message_65:  db "testing for 65",  0
	ISR_message_66:  db "testing for 66",  0
	ISR_message_67:  db "testing for 67",  0
	ISR_message_68:  db "testing for 68",  0
	ISR_message_69:  db "testing for 69",  0
	ISR_message_70:  db "testing for 70",  0
	ISR_message_71:  db "testing for 71",  0
	ISR_message_72:  db "testing for 72",  0
	ISR_message_73:  db "testing for 73",  0
	ISR_message_74:  db "testing for 74",  0
	ISR_message_75:  db "testing for 75",  0
	ISR_message_76:  db "testing for 76",  0
	ISR_message_77:  db "testing for 77",  0
	ISR_message_78:  db "testing for 78",  0
	ISR_message_79:  db "testing for 79",  0
	ISR_message_80:  db "testing for 80",  0
	ISR_message_81:  db "testing for 81",  0
	ISR_message_82:  db "testing for 82",  0
	ISR_message_83:  db "testing for 83",  0
	ISR_message_84:  db "testing for 84",  0
	ISR_message_85:  db "testing for 85",  0
	ISR_message_86:  db "testing for 86",  0
	ISR_message_87:  db "testing for 87",  0
	ISR_message_88:  db "testing for 88",  0
	ISR_message_89:  db "testing for 89",  0
	ISR_message_90:  db "testing for 90",  0
	ISR_message_91:  db "testing for 91",  0
	ISR_message_92:  db "testing for 92",  0
	ISR_message_93:  db "testing for 93",  0
	ISR_message_94:  db "testing for 94",  0
	ISR_message_95:  db "testing for 95",  0
	ISR_message_96:  db "testing for 96",  0
	ISR_message_97:  db "testing for 97",  0
	ISR_message_98:  db "testing for 98",  0
	ISR_message_99:  db "testing for 99",  0
	ISR_message_100: db "testing for 100", 0
	ISR_message_101: db "testing for 101", 0
	ISR_message_102: db "testing for 102", 0
	ISR_message_103: db "testing for 103", 0
	ISR_message_104: db "testing for 104", 0
	ISR_message_105: db "testing for 105", 0
	ISR_message_106: db "testing for 106", 0
	ISR_message_107: db "testing for 107", 0
	ISR_message_108: db "testing for 108", 0
	ISR_message_109: db "testing for 109", 0
	ISR_message_110: db "testing for 110", 0
	ISR_message_111: db "testing for 111", 0
	ISR_message_112: db "testing for 112", 0
	ISR_message_113: db "testing for 113", 0
	ISR_message_114: db "testing for 114", 0
	ISR_message_115: db "testing for 115", 0
	ISR_message_116: db "testing for 116", 0
	ISR_message_117: db "testing for 117", 0
	ISR_message_118: db "testing for 118", 0
	ISR_message_119: db "testing for 119", 0
	ISR_message_120: db "testing for 120", 0
	ISR_message_121: db "testing for 121", 0
	ISR_message_122: db "testing for 122", 0
	ISR_message_123: db "testing for 123", 0
	ISR_message_124: db "testing for 124", 0
	ISR_message_125: db "testing for 125", 0
	ISR_message_126: db "testing for 126", 0
	ISR_message_127: db "testing for 127", 0
	ISR_message_128: db "testing for 128", 0
	ISR_message_129: db "testing for 129", 0
	ISR_message_130: db "testing for 130", 0
	ISR_message_131: db "testing for 131", 0
	ISR_message_132: db "testing for 132", 0
	ISR_message_133: db "testing for 133", 0
	ISR_message_134: db "testing for 134", 0
	ISR_message_135: db "testing for 135", 0
	ISR_message_136: db "testing for 136", 0
	ISR_message_137: db "testing for 137", 0
	ISR_message_138: db "testing for 138", 0
	ISR_message_139: db "testing for 139", 0
	ISR_message_140: db "testing for 140", 0
	ISR_message_141: db "testing for 141", 0
	ISR_message_142: db "testing for 142", 0
	ISR_message_143: db "testing for 143", 0
	ISR_message_144: db "testing for 144", 0
	ISR_message_145: db "testing for 145", 0
	ISR_message_146: db "testing for 146", 0
	ISR_message_147: db "testing for 147", 0
	ISR_message_148: db "testing for 148", 0
	ISR_message_149: db "testing for 149", 0
	ISR_message_150: db "testing for 150", 0
	ISR_message_151: db "testing for 151", 0
	ISR_message_152: db "testing for 152", 0
	ISR_message_153: db "testing for 153", 0
	ISR_message_154: db "testing for 154", 0
	ISR_message_155: db "testing for 155", 0
	ISR_message_156: db "testing for 156", 0
	ISR_message_157: db "testing for 157", 0
	ISR_message_158: db "testing for 158", 0
	ISR_message_159: db "testing for 159", 0
	ISR_message_160: db "testing for 160", 0
	ISR_message_161: db "testing for 161", 0
	ISR_message_162: db "testing for 162", 0
	ISR_message_163: db "testing for 163", 0
	ISR_message_164: db "testing for 164", 0
	ISR_message_165: db "testing for 165", 0
	ISR_message_166: db "testing for 166", 0
	ISR_message_167: db "testing for 167", 0
	ISR_message_168: db "testing for 168", 0
	ISR_message_169: db "testing for 169", 0
	ISR_message_170: db "testing for 170", 0
	ISR_message_171: db "testing for 171", 0
	ISR_message_172: db "testing for 172", 0
	ISR_message_173: db "testing for 173", 0
	ISR_message_174: db "testing for 174", 0
	ISR_message_175: db "testing for 175", 0
	ISR_message_176: db "testing for 176", 0
	ISR_message_177: db "testing for 177", 0
	ISR_message_178: db "testing for 178", 0
	ISR_message_179: db "testing for 179", 0
	ISR_message_180: db "testing for 180", 0
	ISR_message_181: db "testing for 181", 0
	ISR_message_182: db "testing for 182", 0
	ISR_message_183: db "testing for 183", 0
	ISR_message_184: db "testing for 184", 0
	ISR_message_185: db "testing for 185", 0
	ISR_message_186: db "testing for 186", 0
	ISR_message_187: db "testing for 187", 0
	ISR_message_188: db "testing for 188", 0
	ISR_message_189: db "testing for 189", 0
	ISR_message_190: db "testing for 190", 0
	ISR_message_191: db "testing for 191", 0
	ISR_message_192: db "testing for 192", 0
	ISR_message_193: db "testing for 193", 0
	ISR_message_194: db "testing for 194", 0
	ISR_message_195: db "testing for 195", 0
	ISR_message_196: db "testing for 196", 0
	ISR_message_197: db "testing for 197", 0
	ISR_message_198: db "testing for 198", 0
	ISR_message_199: db "testing for 199", 0
	ISR_message_200: db "testing for 200", 0
	ISR_message_201: db "testing for 201", 0
	ISR_message_202: db "testing for 202", 0
	ISR_message_203: db "testing for 203", 0
	ISR_message_204: db "testing for 204", 0
	ISR_message_205: db "testing for 205", 0
	ISR_message_206: db "testing for 206", 0
	ISR_message_207: db "testing for 207", 0
	ISR_message_208: db "testing for 208", 0
	ISR_message_209: db "testing for 209", 0
	ISR_message_210: db "testing for 210", 0
	ISR_message_211: db "testing for 211", 0
	ISR_message_212: db "testing for 212", 0
	ISR_message_213: db "testing for 213", 0
	ISR_message_214: db "testing for 214", 0
	ISR_message_215: db "testing for 215", 0
	ISR_message_216: db "testing for 216", 0
	ISR_message_217: db "testing for 217", 0
	ISR_message_218: db "testing for 218", 0
	ISR_message_219: db "testing for 219", 0
	ISR_message_220: db "testing for 220", 0
	ISR_message_221: db "testing for 221", 0
	ISR_message_222: db "testing for 222", 0
	ISR_message_223: db "testing for 223", 0
	ISR_message_224: db "testing for 224", 0
	ISR_message_225: db "testing for 225", 0
	ISR_message_226: db "testing for 226", 0
	ISR_message_227: db "testing for 227", 0
	ISR_message_228: db "testing for 228", 0
	ISR_message_229: db "testing for 229", 0
	ISR_message_230: db "testing for 230", 0
	ISR_message_231: db "testing for 231", 0
	ISR_message_232: db "testing for 232", 0
	ISR_message_233: db "testing for 233", 0
	ISR_message_234: db "testing for 234", 0
	ISR_message_235: db "testing for 235", 0
	ISR_message_236: db "testing for 236", 0
	ISR_message_237: db "testing for 237", 0
	ISR_message_238: db "testing for 238", 0
	ISR_message_239: db "testing for 239", 0
	ISR_message_240: db "testing for 240", 0
	ISR_message_241: db "testing for 241", 0
	ISR_message_242: db "testing for 242", 0
	ISR_message_243: db "testing for 243", 0
	ISR_message_244: db "testing for 244", 0
	ISR_message_245: db "testing for 245", 0
	ISR_message_246: db "testing for 246", 0
	ISR_message_247: db "testing for 247", 0
	ISR_message_248: db "testing for 248", 0
	ISR_message_249: db "testing for 249", 0
	ISR_message_250: db "testing for 250", 0
	ISR_message_251: db "testing for 251", 0
	ISR_message_252: db "testing for 252", 0
	ISR_message_253: db "testing for 253", 0
	ISR_message_254: db "testing for 254", 0
	ISR_message_255: db "testing for 255", 0

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

