TARGET = psz_psp
OBJS = main.o psz_core.o

INCDIR = ../../include
CFLAGS = -O2 -G0 -Wall -I$(INCDIR)
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

EXTRA_TARGETS = EBOOT.PBP
EXTRA_CLEAN = PARAM.SFO EBOOT.PBP
PSP_EBOOT_TITLE = PSZ Studio
PSP_EBOOT_ICON = ../../assets/psp/icon.png
PSP_EBOOT_PIC1 = ../../assets/psp/pic1.png

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

psz_core.o: ../../src/psz_core.c ../../include/psz.h
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c ../../include/psz.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: eboot
eboot: EBOOT.PBP

clean-assets:
	rm -f $(EXTRA_CLEAN) $(OBJS) $(TARGET).elf
