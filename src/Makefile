OBJ_DIR=./build
SRC_DIR = .

CC = i686-elf-gcc
CFLAGS = -std=c2x -ffreestanding -nostdlib -I './kernel/library' -I './libc' -Wall -Wextra -Wpedantic -ggdb
LDFLAGS = -ffreestanding -nostdlib  -lgcc -ggdb
SRC_C := $(shell find $(SRC_DIR) -type f -name '*.c')
OBJ_C := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_C))

NASM_FLAGS := -gdwarf -f elf32
SRC_NASM := $(shell find $(SRC_DIR) -type f -name '*.nasm')
OBJ_NASM := $(patsubst $(SRC_DIR)/%.nasm, $(OBJ_DIR)/%.nasm.o, $(SRC_NASM))

SRC_GAS := $(shell find -type f -name '*.s')
OBJ_GAS := $(patsubst $(SRC_GAS)%.s, $(OBJ_DIR)%.s.o, $(SRC_GAS))

# Name is hardcoded in /isodir/grub/grub.cfg
OS = isodir/boot/os.bin
OS_ISO = $(OBJ_DIR)/os.iso

.PHONY: clean

all: $(OS_ISO)

$(OS_ISO): $(OBJ_C) $(OBJ_NASM) $(OBJ_GAS)
	@printf "\n"
	$(CC) -T linker.ld $(OBJ_C) $(OBJ_NASM) $(OBJ_GAS) -o $(OS) $(LDFLAGS)
	@printf "\n"
	grub-file --is-x86-multiboot $(OS)
	grub-mkrescue -o $(OS_ISO) isodir

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(shell dirname $@)
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/%.nasm.o: $(SRC_DIR)/%.nasm
	@mkdir -p $(shell dirname $@)
	nasm $< -o $@ $(NASM_FLAGS)

$(OBJ_DIR)/%.s.o: $(SRC_DIR)/%.s
	i686-elf-as $< -o $@

clean:
	rm -rf build

run: all
	qemu-system-i386 -cdrom $(OS_ISO)

debug: all
	qemu-system-i386 -cdrom $(OS_ISO) -S -gdb tcp::26000 -no-shutdown -no-reboot -d int

