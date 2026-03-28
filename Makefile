# 1. Directories & Variables
SRCDIRS = kernel lib
INCDIR  = include
BOOTDIR = boot

CC      = gcc
LD      = ld
ASM     = nasm

CFLAGS  = -m32 -ffreestanding -fno-pie -fno-stack-protector -I$(INCDIR) -c
LDFLAGS = -m elf_i386 -Ttext 0x8000 -e setup_start --oformat binary

# 2. Source & Object Files
C_SRCS   = $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))
ASM_SRCS = $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.asm))

C_OBJS   = $(notdir $(C_SRCS:.c=.o))
ASM_OBJS = $(notdir $(ASM_SRCS:.asm=.o))

KERNEL_OBJS = $(C_OBJS) $(ASM_OBJS)

vpath %.c $(SRCDIRS)
vpath %.asm $(SRCDIRS)

# 3. Phony Targets
.PHONY: all run clean

# 4. Build Rules
all: disk.img

run: disk.img
	qemu-system-x86_64 -drive format=raw,file=$<

disk.img: boot.bin kernel.bin
	cat $^ > $@

kernel.bin: setup.o $(KERNEL_OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

# 5. Compile Rules
boot.bin: $(BOOTDIR)/boot.asm kernel.bin
	@KERNEL_SIZE=$$(stat -c %s kernel.bin); \
	SECTORS=$$(( (KERNEL_SIZE + 511) / 512 )); \
	$(ASM) -f bin $< -D KERNEL_SECTORS=$$SECTORS -o $@

setup.o: $(BOOTDIR)/setup.asm
	$(ASM) -f elf32 $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.asm
	$(ASM) -f elf32 $< -o $@

# 6. Clean
clean:
	rm -rf *.bin *.o *.img