OBJ_DIR    ?= ./build
SRC_DIR    = ./src

DEBUGGER   ?= gf2
QEMU       ?= qemu-system-i386
QEMU_FLAGS ?= -cdrom $(OS_ISO) -audiodev pa,id=speaker -machine pcspk-audiodev=speaker

CC         ?= i686-elf-gcc
# TODO: fix gcc freestanding include bug
# If I don't add `-nostdinc` it seems that gcc will gladly include system libraries even though
# -ffreestanding should disable this. The problem is that if I add -nostdinc I loose access to
# the headers that should be available in freestanding mode like stddef.h. So for now I will
# not use `-nostdinc` and just deal with the bug later
CFLAGS     = -std=c2x -ffreestanding -nostdlib -I '$(SRC_DIR)/kernel' -I '$(SRC_DIR)/libc' -I '$(SRC_DIR)/libc/libk' -MMD -MP -Wall -Wextra -Wpedantic -Wvla -Wimplicit-fallthrough -fanalyzer -ggdb
LDFLAGS    = -ffreestanding -nostdlib -lgcc -ggdb
SRC_C      := $(shell find $(SRC_DIR) -type f -name '*.c')
OBJ_C      := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.c.o, $(SRC_C))

# TODO: Also compile nasm64.asm files if compiliing 64 bit os
NASM32_FLAGS := -gdwarf -f elf32
SRC_NASM32   := $(shell find $(SRC_DIR) -type f -name '*.nasm32.asm')
OBJ_NASM32   := $(patsubst $(SRC_DIR)/%.nasm32.asm, $(OBJ_DIR)/%.nasm32.o, $(SRC_NASM32))

SRC_GAS    := $(shell find $(SRC_DIR) -type f -name '*.S')
OBJ_GAS    := $(patsubst $(SRC_DIR)%.S, $(OBJ_DIR)%.S.o, $(SRC_GAS))

DEPEND     := $(patsubst %.o, %.d, $(OBJ_C))
DEPEND     += $(patsubst %.o, %.d, $(OBJ_GAS))

# Name is hardcoded in /isodir/grub/grub.cfg
OS         = $(SRC_DIR)/isodir/boot/os.bin
OS_ISO     = $(OBJ_DIR)/os.iso

.PHONY: clean

all: $(OS_ISO)

$(OS_ISO): $(OBJ_C) $(OBJ_NASM32) $(OBJ_GAS) Makefile
	@printf "\n"
	$(CC) -T linker.ld $(OBJ_C) $(OBJ_NASM32) $(OBJ_GAS) -o $(OS) $(LDFLAGS)
	@printf "\n"
	grub-file --is-x86-multiboot $(OS)
	grub-mkrescue -o $(OS_ISO) $(SRC_DIR)/isodir

$(OBJ_DIR)/%.c.o: $(SRC_DIR)/%.c
	@mkdir -p $(shell dirname $@)
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/%.nasm32.o: $(SRC_DIR)/%.nasm32.asm
	@mkdir -p $(shell dirname $@)
	nasm $< -o $@ $(NASM32_FLAGS)

$(OBJ_DIR)/%.S.o: $(SRC_DIR)/%.S
	@mkdir -p $(shell dirname $@)
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf $(OBJ_DIR)
	rm $(OS)

run: all
	$(QEMU) $(QEMU_FLAGS)

debug: all
	setsid -f $(QEMU) -S -gdb tcp::26000 -no-shutdown -no-reboot -d int -trace pic* $(QEMU_FLAGS)
	!(pgrep $(DEBUGGER)) && setsid -f $(DEBUGGER) &


-include $(DEPEND)

DOC_DIR    = ./doc

SRC_MD	   := $(shell find $(SRC_DIR) -type f -name '*.md')
OBJ_MD	   := $(patsubst $(SRC_DIR)%.md, $(DOC_DIR)%.pdf, $(SRC_MD))

SRC_MD_2   := $(shell find $(DOC_DIR) -maxdepth 1 -type f -name '*.md')
OBJ_MD     += $(patsubst $(DOC_DIR)%.md, $(DOC_DIR)%.pdf, $(SRC_MD_2))

$(DOC_DIR)/%.pdf: $(SRC_DIR)/%.md
	@mkdir -p $(shell dirname $@)
	pandoc --number-sections -F mermaid-filter $< -o $@

$(DOC_DIR)/%.pdf: $(DOC_DIR)/%.md
	@mkdir -p $(shell dirname $@)
	pandoc --number-sections -F mermaid-filter $< -o $@

doc: $(OBJ_MD) Makefile
	# Workaround to delete garbage made from generating documents
	find -name 'mermaid-filter.err' -exec rm {} \;
