all: kstuff.bin

kstuff.bin: main.asm
	nasm -f bin main.asm -o kstuff.bin

clean:
	rm -f *.bin
