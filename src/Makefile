TARGET = inducer.bin
OBJS = gmtime.o icache.o list.o main.o maple.o swarm.o theme.o image.o loadpng.o intro.o
LIBS = -L$(KOS_BASE)/lib -Lexpat -lexpat -ldcdivx -lpng -lz -ltmalloc -lkallisti -lgcc -lm
KOS_INCS += -Iinclude

all: $(TARGET)

include $(KOS_BASE)/Makefile.rules

clean:
	-rm -f *.elf $(TARGET) $(OBJS)

inducer.elf: $(OBJS)
	$(KOS_CC) $(KOS_CFLAGS) $(KOS_LDFLAGS) -o inducer.elf $(KOS_START) \
		$(OBJS) $(OBJEXTRA) $(LIBS)

%.bin: %.elf
	$(KOS_OBJCOPY) -O binary $< $@

run: $(TARGET)
	$(KOS_LOADER) $(TARGET)

dist:
	rm -f $(OBJS)
	$(KOS_CC_BASE)/bin/sh-elf-strip $(TARGET)

