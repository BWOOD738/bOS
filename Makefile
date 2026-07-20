OS_NAME := bOS
ISO_NAME := $(OS_NAME).iso
KERNEL_BIN := $(OS_NAME)


CC := /home/bw/opt/cross/bin/i686-elf-gcc # Use an absolute path for now
AS := nasm
LD := ld

CFLAGS := -ffreestanding -Wall -Wextra -O2 -g -I.
LDFLAGS := -m elf_i386 -T linker.ld
ASFLAGS := -f elf32

# --- File Discovery ---
# This recursively finds all .c and .s files in the current directory and subdirectories
C_SOURCES := $(shell find . -name "*.c")
S_SOURCES := $(shell find . -name "*.s")

# Convert source file lists to object file lists in a 'build' directory
# This keeps your source tree clean
C_OBJECTS := $(patsubst %.c, build/%.o, $(C_SOURCES))
S_OBJECTS := $(patsubst %.s, build/%.o, $(S_SOURCES))
OBJECTS   := $(C_OBJECTS) $(S_OBJECTS)

# --- Rules ---

.PHONY: all clean iso run

all: $(KERNEL_BIN)

# Link the kernel
$(KERNEL_BIN): $(OBJECTS)
	@echo "Linking $@"
	@$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

# Compile C files
build/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Assemble assembly files
build/%.o: %.s
	@mkdir -p $(dir $@)
	@echo "Assembling $<"
	@$(AS) $(ASFLAGS) $< -o $@

# Create the ISO image using GRUB
iso: $(KERNEL_BIN)
	@mkdir -p isodir/boot/grub
	@cp $(KERNEL_BIN) isodir/boot/$(KERNEL_BIN)
	@cp boot/grub/grub.cfg isodir/boot/grub/grub.cfg
	@grub-mkrescue -o $(ISO_NAME) isodir
	@rm -rf isodir
	@echo "ISO created: $(ISO_NAME)"

# Clean up build artifacts
clean:
	@rm -rf build $(KERNEL_BIN) $(ISO_NAME)
	@echo "Cleaned."

debug:
	qemu-system-i386 -cdrom $(ISO_NAME) -s -S

# Optional: Run in QEMU
run: iso
	qemu-system-i386 $(ISO_NAME) -d int -m 128M