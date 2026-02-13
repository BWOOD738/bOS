
BOOT_DIR=boot

all:
	make -C boot 
qemu:
	qemu-system-i386 boot/boot.bin

qemu_debug:
	qemu-system-i386 boot/boot.bin -gdb tcp::1234 -S 

clean:
	rm -f $(BOOT_DIR)/*.bin