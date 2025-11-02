# Makefile for VIN OS
ASM = nasm
CC = gcc
LD = ld

ASMFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -fno-pie -I.
LDFLAGS = -m elf_i386 -T linker.ld

KERNEL_OBJS = entry.o kernel.o window.o keyboard.o menu.o apps.o filesystem.o

all: os.bin

bootloader.bin: boot/bootloader.asm
	@echo "Building bootloader..."
	$(ASM) -f bin $< -o $@

entry.o: kernel/entry.asm
	@echo "Building entry point..."
	$(ASM) $(ASMFLAGS) $< -o $@

kernel.o: kernel/kernel.c
	@echo "Building kernel..."
	$(CC) $(CFLAGS) -c $< -o $@

window.o: kernel/window.c
	@echo "Building window manager..."
	$(CC) $(CFLAGS) -c $< -o $@

keyboard.o: kernel/keyboard.c
	@echo "Building keyboard driver..."
	$(CC) $(CFLAGS) -c $< -o $@

menu.o: kernel/menu.c
	@echo "Building menu system..."
	$(CC) $(CFLAGS) -c $< -o $@

apps.o: kernel/apps.c
	@echo "Building applications..."
	$(CC) $(CFLAGS) -c $< -o $@

filesystem.o: kernel/filesystem.c
	@echo "Building file system..."
	$(CC) $(CFLAGS) -c $< -o $@

kernel.bin: $(KERNEL_OBJS)
	@echo "Linking kernel..."
	$(LD) $(LDFLAGS) -o $@ $^

os.bin: bootloader.bin kernel.bin
	@echo "Creating OS image..."
	cat bootloader.bin kernel.bin > os.bin
	@# Pad to ensure we have enough sectors
	@truncate -s 32768 os.bin
	@echo "Build complete! (32KB image)"

clean:
	@echo "Cleaning..."
	rm -f *.o *.bin

run: os.bin
	@echo "Running in QEMU..."
	qemu-system-i386 -drive format=raw,file=os.bin

.PHONY: all clean run