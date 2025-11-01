ARCH = x86_64-elf-
CC = $(ARCH)gcc
LD = $(ARCH)ld
OBJCOPY = $(ARCH)objcopy

BUILD = ./build
SRC   = ./src

# Kernel name, used for the build output
KERNEL_NAME = Snailix

ENTRYPOINT = 0x10000

# gcc params
CFLAGS = -m32			# 32-bit program
CFLAGS += -fno-builtin	# No built-in functions of gcc
CFLAGS += -nostdinc		# No standard header file
CFLAGS += -fno-pic 		# No position-independent code
CFLAGS += -fno-pie		# No position-independent executable
CFLAGS += -nostdlib		# No standard library
CFLAGS += -fno-stack-protector	# No stack protect
CFLAGS := $(strip ${CFLAGS})

DEBUG = -g
INCLUDE = -I$(SRC)/include

# ld params
LDFLAGS := -m elf_i386 \
		   -static \
		   -Ttext $(ENTRYPOINT) 
LDFLAGS := $(strip ${LDFLAGS})

# QEMU params
QEMU := qemu-system-i386 \
	-m 32M \
	-rtc base=localtime \

QEMU_DISK := -boot c \
	-drive file=$(BUILD)/$(KERNEL_NAME).img,if=ide,index=0,media=disk,format=raw \

QEMU_DEBUG := -s -S

# Kernel objects
KERNEL_OBJS = $(BUILD)/kernel/start.o\
			$(BUILD)/kernel/main.o \
			$(BUILD)/kernel/asmfuncs.o \
			$(BUILD)/kernel/screen.o \
			$(BUILD)/kernel/printk.o \
			$(BUILD)/kernel/assert.o \
			$(BUILD)/kernel/memory.o \
			$(BUILD)/kernel/global.o \
			$(BUILD)/lib/string.o \
			$(BUILD)/lib/vsprintf.o \
			$(BUILD)/lib/list.o \
			$(BUILD)/lib/bitmap.o \
	

#===========================================================
# Build commands
#===========================================================
.PHONY: all
all: $(BUILD)/$(KERNEL_NAME).img

.PHONY: run
run: $(BUILD)/$(KERNEL_NAME).img
	$(QEMU) $(QEMU_DISK)

.PHONY: clean
clean:
	rm -rf $(BUILD)

.PHONY: debug
debug: $(BUILD)/$(KERNEL_NAME).img
	$(QEMU) $(QEMU_DISK) $(QEMU_DEBUG)

.PHONY: bochs
bochs: $(BUILD)/$(KERNEL_NAME).img
	bochs -q

#===========================================================
# Build rules
#===========================================================
$(BUILD)/raw.bin: $(KERNEL_OBJS)
	$(shell mkdir -p $(dir $@))
	$(LD) $(LDFLAGS) -static $(KERNEL_OBJS) -o $(BUILD)/raw.bin

$(BUILD)/$(KERNEL_NAME).bin: $(BUILD)/raw.bin
	$(OBJCOPY) -O binary $< $@

$(BUILD)/$(KERNEL_NAME).map: $(BUILD)/raw.bin
	nm $< | sort > $@

$(BUILD)/boot/%.bin: $(SRC)/boot/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f bin $< -o $@

$(BUILD)/%.o: $(SRC)/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f elf32 $(DEBUG) $< -o $@

$(BUILD)/%.o: $(SRC)/%.c
	$(shell mkdir -p $(dir $@))
	$(CC) $(CFLAGS) $(DEBUG) $(INCLUDE) -c $< -o $@

$(BUILD)/$(KERNEL_NAME).img: $(BUILD)/boot/boot.bin $(BUILD)/boot/loader.bin $(BUILD)/$(KERNEL_NAME).bin
	@dd if=/dev/zero of=$@ bs=1M count=16
	@dd if=$(BUILD)/boot/boot.bin of=$@ bs=512 count=1 conv=notrunc
	@dd if=$(BUILD)/boot/loader.bin of=$@ bs=512 count=4 seek=2 conv=notrunc
	@dd if=$(BUILD)/$(KERNEL_NAME).bin of=$@ bs=512 count=200 seek=10 conv=notrunc
