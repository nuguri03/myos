KERNEL_SRCS = $(wildcard kernel/*.c)
KERNEL_OBJS = $(KERNEL_SRCS:kernel/%.c=%.o)

all: disk.img
	qemu-system-x86_64 -drive format=raw,file=disk.img

boot.bin: boot/boot.asm kernel.bin
	@KERNEL_SIZE=$$(stat -c %s kernel.bin); \
	SECTORS=$$(( (KERNEL_SIZE + 511) / 512 )); \
	nasm -f bin boot/boot.asm -D KERNEL_SECTORS=$$SECTORS -o boot.bin

setup.o: boot/setup.asm
	nasm -f elf32 boot/setup.asm -o setup.o
	
%.o: kernel/%.c
	gcc -m32 -ffreestanding -fno-pie -c $< -o $@

kernel.bin: setup.o $(KERNEL_OBJS)
	ld -m elf_i386 -Ttext 0x8000 -e setup_start --oformat binary $^ -o kernel.bin

disk.img: kernel.bin boot.bin
	cat boot.bin kernel.bin > disk.img

clean:
	rm -rf *.bin *.o *.img