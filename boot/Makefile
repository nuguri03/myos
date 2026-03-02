BOOT = boot.asm
SETUP = setup.asm
BOOT_BIN = boot.bin
SETUP_BIN = setup.bin
DISK_IMG = disk.img

all: $(DISK_IMG)
	qemu-system-x86_64 -drive format=raw,file=$(DISK_IMG)

$(BOOT_BIN): $(BOOT)
	nasm -f bin $(BOOT) -o $(BOOT_BIN)

$(SETUP_BIN): $(SETUP)
	nasm -f bin $(SETUP) -o $(SETUP_BIN)
	
$(DISK_IMG): $(BOOT_BIN) $(SETUP_BIN)
	cat $(BOOT_BIN) $(SETUP_BIN) > $(DISK_IMG) 

clean:
	rm -rf *.bin *.img