all: disk.img
	qemu-system-x86_64 -drive format=raw,file=disk.img

boot.bin: boot/boot.asm
	$(eval KERNEL_SIZE = $(shell stat -c %s kernel.bin))
# 올림을 하기 위해 511을 더한다
	$(eval SECTORS = $(shell echo $$(( ($(KERNEL_SIZE) + 511) / 512 ))))
	nasm -f bin boot/boot.asm -D KERNEL_SECTORS=$(SECTORS) -o boot.bin

setup.o: boot/setup.asm
	nasm -f elf32 boot/setup.asm -o setup.o
	
main.o: main.c
	gcc -m32 -ffreestanding -fno-pie -c main.c -o main.o

kernel.bin: setup.o main.o
	ld -m elf_i386 -Ttext 0x8000 -e setup_start --oformat binary setup.o main.o -o kernel.bin

disk.img: kernel.bin boot.bin
	cat boot.bin kernel.bin > disk.img

clean:
	rm -rf *.bin *.o *.img