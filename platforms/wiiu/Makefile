ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path>")
endif

include $(DEVKITPPC)/wiiu_rules

TARGET		:= psz_wiiu
CFLAGS		:= -O2 -Wall -I../../include $(WIIU_CFLAGS)
LDFLAGS		:= $(WIIU_LDFLAGS) -lwhb -lcoreinit -lm

all: $(TARGET).rpx

$(TARGET).elf: main.c ../../src/psz_core.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

%.rpx: %.elf
	rpx-tool $< $@

clean:
	rm -f $(TARGET).elf $(TARGET).rpx ../../src/psz_core.o
