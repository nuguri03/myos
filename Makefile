# 1. Directories & Variables
SRCDIRS = kernel drivers lib
INCDIR  = include
BOOTDIR = boot
OBJDIR  = build
C_OBJDIR   = $(OBJDIR)/c
ASM_OBJDIR = $(OBJDIR)/asm

# 환경 감지 (Termux vs 일반 Ubuntu)
# i686-linux-gnu-gcc 존재 여부로 판단
X86_CROSS = $(shell command -v i686-linux-gnu-gcc 2>/dev/null)

ifneq ($(X86_CROSS),)
    # Termux/proot-distro 환경
    CC = i686-linux-gnu-gcc
    LD = i686-linux-gnu-ld
    # VNC 옵션 추가
    QEMU_FLAGS = -vnc :1 -audiodev none,id=snd0
else
    # 일반 Ubuntu 환경
    CC = gcc
    LD = ld
    # 일반 환경은 GUI 바로 띄움
    QEMU_FLAGS =
endif

ASM  = nasm
# 32비트 커널이므로 i386 에뮬레이터가 더 적합함
QEMU = qemu-system-i386

ARCH_FLAGS      = -m32 -fno-pic -fno-pie
FREESTAND_FLAGS = -ffreestanding -nostdlib -fno-builtin -fno-stack-protector
# -MMD -MP: 헤더 파일 수정 시 자동으로 다시 빌드되게 함
DEBUG_FLAGS     = -g -I$(INCDIR) -MMD -MP

CFLAGS  = $(ARCH_FLAGS) $(FREESTAND_FLAGS) $(DEBUG_FLAGS) -c
LDFLAGS = -m elf_i386 -Ttext 0x8000 -e setup_start --oformat binary

# 2. Source & Object Files
C_SRCS   = $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))
ASM_SRCS = $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.asm))

# kernel/isr.c   → build/c/isr.o
# kernel/isr.asm → build/asm/isr.o
C_OBJS   = $(patsubst %.c,   $(C_OBJDIR)/%.o,   $(notdir $(C_SRCS)))
ASM_OBJS = $(patsubst %.asm, $(ASM_OBJDIR)/%.o, $(notdir $(ASM_SRCS)))
KERNEL_OBJS = $(C_OBJS) $(ASM_OBJS)

# .d 파일은 build/c/ 에 생성됨
DEPS = $(C_OBJS:.o=.d)

vpath %.c   $(SRCDIRS)
vpath %.asm $(SRCDIRS)

# 3. Phony Targets
.PHONY: all run clean

# 4. Build Rules
all: disk.img

run: disk.img
	$(QEMU) -drive format=raw,file=$< $(QEMU_FLAGS) -serial stdio

disk.img: boot.bin kernel.bin
	cat $^ > $@

kernel.bin: $(OBJDIR)/setup.o $(KERNEL_OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

# 5. Compile Rules
boot.bin: $(BOOTDIR)/boot.asm kernel.bin
	@KERNEL_SIZE=$$(stat -c %s kernel.bin); \
	SECTORS=$$(( (KERNEL_SIZE + 511) / 512 )); \
	$(ASM) -f bin $< -D KERNEL_SECTORS=$$SECTORS -o $@

$(OBJDIR)/setup.o: $(BOOTDIR)/setup.asm
	@mkdir -p $(dir $@)
	$(ASM) -f elf32 $< -o $@

$(C_OBJDIR)/%.o: %.c
	@mkdir -p $(C_OBJDIR)
	$(CC) $(CFLAGS) $< -o $@

$(ASM_OBJDIR)/%.o: %.asm
	@mkdir -p $(ASM_OBJDIR)
	$(ASM) -f elf32 $< -o $@

# 6. Clean
clean:
	rm -rf $(OBJDIR) *.bin *.img

# 7. Dependencies (헤더 파일 자동 추적)
-include $(DEPS)