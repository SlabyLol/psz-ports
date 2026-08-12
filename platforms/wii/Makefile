.SUFFIXES:

ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment.")
endif

include $(DEVKITPPC)/wii_rules

TARGET := psz_wii
SOURCES := main.c ../../src/psz_core.c
CFLAGS := -g -O2 -Wall $(MACHDEP) -I../../include -I$(LIBOGC_INC)
LDFLAGS := -g $(MACHDEP) -L$(LIBOGC_LIB)
LIBS := -lwiiuse -lbte -lfat -logc -lm

.PHONY: all clean
all: $(TARGET).dol

$(TARGET).elf: $(SOURCES) ../../include/psz.h
	$(CC) $(CFLAGS) $(SOURCES) $(LDFLAGS) $(LIBS) -o $@

$(TARGET).dol: $(TARGET).elf
	elf2dol $< $@

clean:
	rm -f $(TARGET).elf $(TARGET).dol $(TARGET).map
