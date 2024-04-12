BUILD_DIR=./build

SRCDIR = .
SRC := $(shell find $(SRCDIR) -type f -name '*.c')
OBJ := $(patsubst $(SRCDIR)/%.c, $(BUILD_DIR)/%.o, $(SRC))

MULTIBOOT_HEADER = kernel/multiboot.s
MULTIBOOT_HEADER_OBJ = $(BUILD_DIR)/kernel/multiboot.o

OS_ISO = $(BUILD_DIR)/os.iso
# Name is hardcoded in /isodir/grub/grub.cfg
OS = isodir/boot/os.bin

.PHONY: run clean

all: $(OBJ) | $(MULTIBOOT_HEADER_OBJ)
	i686-elf-gcc -T linker.ld -o $(OS) -ffreestanding -ggdb -nostdlib $(OBJ) $(MULTIBOOT_HEADER_OBJ) -lgcc
	grub-file --is-x86-multiboot $(OS)
	grub-mkrescue -o $(OS_ISO) isodir

$(BUILD_DIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(shell dirname $@)
	i686-elf-gcc -c $< -o $@ -std=c2x -ffreestanding -nostdlib -ggdb -Wall -Wextra -Wpedantic

$(MULTIBOOT_HEADER_OBJ): $(MULTIBOOT_HEADER)
	i686-elf-as $< -o $@

clean:
	rm -rf build

run: all
	qemu-system-i386 -cdrom $(OS_ISO)

debug: all
	qemu-system-i386 -cdrom $(OS_ISO) -S -gdb tcp::26000

