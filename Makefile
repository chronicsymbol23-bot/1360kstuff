CC      := gcc
OBJCOPY := objcopy
CFLAGS  := -O2 -nostdlib -fno-builtin -fno-stack-protector -Wall -Wextra -fPIC

all: kstuff.bin

kstuff.bin: main.elf
	$(OBJCOPY) -O binary -j .text -j .rodata -j .data main.elf kstuff.bin

main.elf: main.c
	$(CC) $(CFLAGS) -Ttext 0x0 -nostartfiles -e init_1360_fpkg_bypass main.c -o main.elf

clean:
	rm -f *.elf *.bin
