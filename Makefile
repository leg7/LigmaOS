OBJ_DIR    ?= ./build
SRC_DIR    = ./src

DEBUGGER   ?= gf2
QEMU	   ?= qemu-system-i386

CC         ?= i686-elf-gcc
# TODO: fix gcc freestanding include bug
# If I don't add `-nostdinc` it seems that gcc will gladly include system libraries even though
# -ffreestanding should disable this. The problem is that if I add -nostdinc I loose access to
# the headers that should be available in freestanding mode like stddef.h. So for now I will
# not use `-nostdinc` and just deal with the bug later
CFLAGS     = -std=c2x -ffreestanding -nostdlib -I '$(SRC_DIR)/kernel' -I '$(SRC_DIR)/libc' -I '$(SRC_DIR)/libc/libk' -Wall -Wextra -Wpedantic -Wvla -Wimplicit-fallthrough -fanalyzer -ggdb
LDFLAGS    = -ffreestanding -nostdlib -lgcc -ggdb
SRC_C      := $(shell find $(SRC_DIR) -type f -name '*.c')
OBJ_C      := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_C))
SRC_C_SOLO_HEADERS := $(shell find $(SRC_DIR) -name '*.h' ! -exec sh -c 'c=$$(echo {} | sed s/.h$$/.c/); test -f $$c' sh '{}' \; -print)

# TODO: Also compile nasm64.asm files if compiliing 64 bit os
NASM32_FLAGS := -gdwarf -f elf32
SRC_NASM32   := $(shell find $(SRC_DIR) -type f -name '*.nasm32.asm')
OBJ_NASM32   := $(patsubst $(SRC_DIR)/%.nasm32.asm, $(OBJ_DIR)/%.nasm32.o, $(SRC_NASM32))

SRC_GAS    := $(shell find $(SRC_DIR) -type f -name '*.S')
OBJ_GAS    := $(patsubst $(SRC_DIR)%.S, $(OBJ_DIR)%.S.o, $(SRC_GAS))

# Name is hardcoded in /isodir/grub/grub.cfg
OS         = $(SRC_DIR)/isodir/boot/os.bin
OS_ISO     = $(OBJ_DIR)/os.iso

.PHONY: clean run debug

all: $(OS_ISO)

$(OS_ISO): $(OBJ_C) $(OBJ_NASM32) $(OBJ_GAS)
	@printf "\n"
	$(CC) -T linker.ld $(OBJ_C) $(OBJ_NASM32) $(OBJ_GAS) -o $(OS) $(LDFLAGS)
	@printf "\n"
	@echo $(SRC_C_SOLO_HEADERS)
	grub-file --is-x86-multiboot $(OS)
	grub-mkrescue -o $(OS_ISO) $(SRC_DIR)/isodir

# TODO:
# A Translation unit A.o is recompiled if A.c or A.h were changed
# However all the C files will have to be recompiled if one of the standalone headers was modified
# This isn't very smart, the best way would be to recompile only the translation units that include
# these standalone headers but I don't see a generic way to do this in Make.
# For clarity let's put these standalone headers in src/kernel/library.
#
# Also this recipe forces every .c file to have a respectively named .h file otherwise the translation
# unit won't be recompiled if the .c file changes.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/%.h Makefile $(SRC_C_SOLO_HEADERS)
	@mkdir -p $(shell dirname $@)
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/%.nasm32.o: $(SRC_DIR)/%.nasm32.asm Makefile
	@mkdir -p $(shell dirname $@)
	nasm $< -o $@ $(NASM32_FLAGS)

$(OBJ_DIR)/%.S.o: $(SRC_DIR)/%.S Makefile $(SRC_C_SOLO_HEADERS)
	@mkdir -p $(shell dirname $@)
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf $(OBJ_DIR)
	rm $(OS)

run: all
	$(QEMU) -cdrom $(OS_ISO)

debug: all
	setsid -f $(QEMU) -cdrom $(OS_ISO) -S -gdb tcp::26000 -no-shutdown -no-reboot -d int -trace pic*
	!(pgrep $(DEBUGGER)) && setsid -f $(DEBUGGER) &

