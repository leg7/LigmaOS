[bits 32]
global IdtLoadi686

IdtLoadi686:
	lidt [esp + 8]
	ret
