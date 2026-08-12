ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path>")
endif

include $(DEVKITPPC)/wiiu_rules

TARGET		:= psz_wiiu
CFLAGS		:= -O2 -Wall -I../../include $(WIIU_CFLAGS)
LDFLAGS		:= $(WIIU_LDFLAGS) -lwhb -lcoreinit -lm

all: $(TARGET).rpx $(TARGET).wuhb

$(TARGET).elf: main.c ../../src/psz_core.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

%.rpx: %.elf
	rpx-tool $< $@

%.wuhb: %.rpx
	wuhbtool $< $@ \
		--name="PSZ Studio" \
		--author="Manus AI" \
		--icon=../../assets/wiiu/icon.png \
		--banner=../../assets/wiiu/banner.png

clean:
	rm -f $(TARGET).elf $(TARGET).rpx $(TARGET).wuhb ../../src/psz_core.o
