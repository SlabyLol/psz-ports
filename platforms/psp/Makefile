TARGET = psz_psp
OBJS = main.c ../../src/psz_core.o

CFLAGS = -O2 -G0 -Wall -I../../include
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

PSPSDK = $(shell psp-config --pspsdk-dir)
include $(PSPSDK)/lib/build.mak
