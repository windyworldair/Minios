CC := gcc
AS := gcc
LD := ld

CFLAGS := -std=c17 -m64 -ffreestanding -fno-pic -fno-stack-protector -mno-red-zone -Wall -Wextra -Werror -I./src
ASFLAGS := -m64 -ffreestanding
LDFLAGS := -m elf_x86_64 -T linker.ld -nostdlib

BUILD_DIR := build
SRC_DIR := src

C_SOURCES := $(SRC_DIR)/kernel.c $(SRC_DIR)/gfx.c $(SRC_DIR)/apps.c $(SRC_DIR)/keyboard.c $(SRC_DIR)/mouse.c $(SRC_DIR)/memory.c $(SRC_DIR)/ramfs.c $(SRC_DIR)/interrupts.c $(SRC_DIR)/ata.c $(SRC_DIR)/pfs.c
ASM_SOURCES := $(SRC_DIR)/boot.s

OBJECTS := $(BUILD_DIR)/kernel.o $(BUILD_DIR)/gfx.o $(BUILD_DIR)/apps.o $(BUILD_DIR)/keyboard.o $(BUILD_DIR)/mouse.o $(BUILD_DIR)/memory.o $(BUILD_DIR)/ramfs.o $(BUILD_DIR)/interrupts.o $(BUILD_DIR)/ata.o $(BUILD_DIR)/pfs.o $(BUILD_DIR)/boot.o
KERNEL := $(BUILD_DIR)/kernel.elf

.PHONY: all clean iso run

all: $(KERNEL)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/kernel.o: $(SRC_DIR)/kernel.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/gfx.o: $(SRC_DIR)/gfx.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/apps.o: $(SRC_DIR)/apps.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/keyboard.o: $(SRC_DIR)/keyboard.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/mouse.o: $(SRC_DIR)/mouse.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


$(BUILD_DIR)/memory.o: $(SRC_DIR)/memory.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/ramfs.o: $(SRC_DIR)/ramfs.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


$(BUILD_DIR)/interrupts.o: $(SRC_DIR)/interrupts.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/ata.o: $(SRC_DIR)/ata.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/pfs.o: $(SRC_DIR)/pfs.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/boot.o: $(SRC_DIR)/boot.s | $(BUILD_DIR)
	$(AS) $(ASFLAGS) -c $< -o $@

$(KERNEL): $(OBJECTS) linker.ld
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

iso: $(KERNEL)
	mkdir -p $(BUILD_DIR)/iso/boot/grub
	cp $(KERNEL) $(BUILD_DIR)/iso/boot/kernel.elf
	printf 'set timeout=0\nset default=0\nmenuentry "MiniOS64" {\n  multiboot2 /boot/kernel.elf\n  boot\n}\n' > $(BUILD_DIR)/iso/boot/grub/grub.cfg
	grub-mkrescue -o $(BUILD_DIR)/minios64.iso $(BUILD_DIR)/iso

run: iso
	qemu-system-x86_64 -cdrom $(BUILD_DIR)/minios64.iso

clean:
	rm -rf $(BUILD_DIR)


OBJCOPY := objcopy

BOOT_DIR := bootloader
BOOT_BUILD := $(BUILD_DIR)/bootloader

.PHONY: bios_boot bios_image

$(BOOT_BUILD):
	mkdir -p $(BOOT_BUILD)

$(BOOT_BUILD)/stage1.o: $(BOOT_DIR)/stage1.s | $(BOOT_BUILD)
	$(AS) -m32 -ffreestanding -c $< -o $@

$(BOOT_BUILD)/stage2.o: $(BOOT_DIR)/stage2.s | $(BOOT_BUILD)
	$(AS) -m64 -ffreestanding -c $< -o $@

$(BOOT_BUILD)/stage1.elf: $(BOOT_BUILD)/stage1.o
	$(LD) -m elf_i386 -Ttext 0x7C00 --oformat elf32-i386 -nostdlib $< -o $@

$(BOOT_BUILD)/stage2.elf: $(BOOT_BUILD)/stage2.o
	$(LD) -m elf_x86_64 -Ttext 0x0000 --oformat elf64-x86-64 -nostdlib $< -o $@

$(BOOT_BUILD)/stage1.bin: $(BOOT_BUILD)/stage1.elf
	$(OBJCOPY) -O binary $< $@

$(BOOT_BUILD)/stage2.bin: $(BOOT_BUILD)/stage2.elf
	$(OBJCOPY) -O binary $< $@

# Builds a custom BIOS boot path image independent from GRUB.
bios_image: $(BOOT_BUILD)/stage1.bin $(BOOT_BUILD)/stage2.bin
	dd if=/dev/zero of=$(BUILD_DIR)/minios-bios.img bs=512 count=2880
	dd if=$(BOOT_BUILD)/stage1.bin of=$(BUILD_DIR)/minios-bios.img conv=notrunc
	dd if=$(BOOT_BUILD)/stage2.bin of=$(BUILD_DIR)/minios-bios.img bs=512 seek=1 conv=notrunc

# Runs the custom bootloader image.
bios_boot: bios_image
	qemu-system-i386 -fda $(BUILD_DIR)/minios-bios.img


.PHONY: bios_full_image

# Unified BIOS image layout: stage1, stage2, then kernel payload sectors.
bios_full_image: bios_image $(KERNEL)
	objcopy -O binary $(KERNEL) $(BUILD_DIR)/kernel_payload.bin
	dd if=$(BUILD_DIR)/kernel_payload.bin of=$(BUILD_DIR)/minios-bios.img bs=512 seek=64 conv=notrunc
	printf "stage1_lba=0\nstage2_lba=1\nkernel_lba=64\n" > $(BUILD_DIR)/bios_layout.txt


$(BOOT_BUILD)/test_payload.o: $(BOOT_DIR)/test_payload.s | $(BOOT_BUILD)
	$(AS) -m32 -ffreestanding -c $< -o $@

$(BOOT_BUILD)/test_payload.elf: $(BOOT_BUILD)/test_payload.o
	$(LD) -m elf_i386 -Ttext 0x00080000 --oformat elf32-i386 -nostdlib $< -o $@

$(BOOT_BUILD)/test_payload.bin: $(BOOT_BUILD)/test_payload.elf
	$(OBJCOPY) -O binary $< $@

.PHONY: bios_test_image vbox_test_image

bios_test_image: bios_image $(BOOT_BUILD)/test_payload.bin
	dd if=$(BOOT_BUILD)/test_payload.bin of=$(BUILD_DIR)/minios-bios.img bs=512 seek=64 conv=notrunc
	cp $(BUILD_DIR)/minios-bios.img $(BUILD_DIR)/minios-vbox-test.img

vbox_test_image: bios_test_image
	@if command -v VBoxManage >/dev/null 2>&1; then 		VBoxManage convertfromraw $(BUILD_DIR)/minios-vbox-test.img $(BUILD_DIR)/minios-vbox-test.vdi --format VDI; 	else 		echo "VBoxManage not found; use build/minios-vbox-test.img directly in VirtualBox as raw disk."; 	fi
