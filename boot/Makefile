ASFLAGS = -f bin 
SRC = $(wildcard *.asm)
BINS = $(SRC:.asm=.bin)


all: $(BINS)

%.bin: %.asm
	nasm $(ASFLAGS) boot.asm -o boot.bin

clean:
	rm -f $(BINS)

.PHONY: all clean