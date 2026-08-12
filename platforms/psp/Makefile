TARGET = psz_psp
OBJS = main.c ../../src/psz_core.o

CFLAGS = -O2 -G0 -Wall -I../../include
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

EXTRA_CLEAN = PARAM.SFO EBOOT.PBP

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

# Custom target to bundle icons and banners into EBOOT.PBP
eboot: $(TARGET).elf
	pack-pbp EBOOT.PBP PARAM.SFO ../../assets/psp/icon.png NULL NULL NULL ../../assets/psp/pic1.png NULL $(TARGET).elf NULL
