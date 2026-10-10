# Directories
# =========================================

SRCDIRS := kernel driver lib

INCDIR  := include
BOOTDIR := boot
OBJDIR  := build

C_OBJDIR   := $(OBJDIR)/c
ASM_OBJDIR := $(OBJDIR)/asm


# Output Files
# =========================================

IMAGE      := myos.img
BOOT_BIN   := boot.bin
KERNEL_BIN := kernel.bin
SETUP_OBJ  := $(OBJDIR)/setup.o


# Tools
# =========================================

CC   := gcc
AS   := nasm
LD   := ld
QEMU := qemu-system-i386


# Flags
# =========================================

CPPFLAGS := \
	-I$(INCDIR) \
	-MMD \
	-MP

CFLAGS := \
	-std=c99 \
	-m32 \
	-march=i386 \
	-fno-pic \
	-fno-pie \
	-ffreestanding \
	-fno-builtin \
	-fno-stack-protector \
	-g

LDFLAGS := \
	-m elf_i386 \
	-T kernel.ld \
	--oformat binary

ASFLAGS := -f elf32

QEMU_FLAGS := \
	-vnc 0.0.0.0:1 \
	-audiodev none,id=noaudio


# Source Files
# =========================================

C_SRCS := \
	$(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.c))

ASM_SRCS := \
	$(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.asm))


# Object Files
# =========================================

C_OBJS := \
	$(patsubst %.c,$(C_OBJDIR)/%.o,$(C_SRCS))

ASM_OBJS := \
	$(patsubst %.asm,$(ASM_OBJDIR)/%.o,$(ASM_SRCS))

KERNEL_OBJS := $(C_OBJS) $(ASM_OBJS)

DEPS := $(C_OBJS:.o=.d)


# Phony Targets
# =========================================

.PHONY: all run clean


# Main Targets
# =========================================

all: $(IMAGE)

run: $(IMAGE)
	$(QEMU) \
		-drive format=raw,file=$(IMAGE) \
		$(QEMU_FLAGS) \
		-serial stdio


# OS Image
# =========================================

$(IMAGE): $(BOOT_BIN) $(KERNEL_BIN)
	cat $^ > $@


# Kernel
# =========================================

$(KERNEL_BIN): $(SETUP_OBJ) $(KERNEL_OBJS)
	$(LD) $(LDFLAGS) $^ -o $@


# Bootloader
# =========================================

$(BOOT_BIN): $(BOOTDIR)/boot.asm $(KERNEL_BIN)
	@KERNEL_SIZE=$$(stat -c %s $(KERNEL_BIN)); \
	SECTORS=$$(( (KERNEL_SIZE + 511) / 512 )); \
	$(AS) -f bin $< \
		-D KERNEL_SECTORS=$$SECTORS \
		-o $@


# Setup
# =========================================

$(SETUP_OBJ): $(BOOTDIR)/setup.asm
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@


# C
# =========================================

$(C_OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@


# Assembly
# =========================================

$(ASM_OBJDIR)/%.o: %.asm
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@


# Dependencies
# =========================================

-include $(DEPS)


# Clean
# =========================================

clean:
	rm -rf $(OBJDIR) $(BOOT_BIN) $(KERNEL_BIN) $(IMAGE)