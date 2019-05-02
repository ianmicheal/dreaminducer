AS = sh-elf-as -little
LD = sh-elf-ld -EL
CC = sh-elf-gcc -ml -m4-single-only -Wall
OBJCOPY = sh-elf-objcopy

OBJS = startup.o main.o cdfs.o syscall.o

binload.bin : binload.elf
	$(OBJCOPY) -O binary $< $@

binload.elf : $(OBJS)
	$(CC) -o $@ $(OBJS) -Wl,-Ttext=0x8cc0b800 -nostartfiles -nostdlib -lgcc

main.o : main.c

syscall.o : syscall.s

startup.o : startup.s

cdfs.o : cdfs.c

clean:
	$(RM) $(OBJS) binload.elf
