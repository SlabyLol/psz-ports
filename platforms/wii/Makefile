ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path>")
endif

include $(DEVKITPPC)/wii_rules

TARGET		:= psz_wii
CFLAGS		:= -O2 -Wall -I../../include -I$(DEVKITPRO)/libogc/include
LDFLAGS		:= -L$(DEVKITPRO)/libogc/lib -lwiiuse -lbte -logc -lm

all: $(TARGET).dol

$(TARGET).elf: main.c ../../src/psz_core.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

%.dol: %.elf
	elf2dol $< $@

clean:
	rm -f $(TARGET).elf $(TARGET).dol ../../src/psz_core.o
