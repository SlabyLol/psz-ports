TARGET		:= psz_3ds
BUILD		:= build
SOURCES		:= . ../../src
DATA		:= data
INCLUDES	:= ../../include

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path> to devkitARM")
endif

include $(DEVKITARM)/3ds_rules

ARCH	:=	-march=armv6k -mtune=mpcore -mfloat-abi=hard
CFLAGS	:=	-Wall -O2 -mword-relocations \
			-fomit-frame-pointer -ffunction-sections \
			$(ARCH) -D_ROMFS -I$(INCLUDES)

CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	:=	-specs=3dsx.specs $(ARCH) -Wl,--gc-sections

LIBS	:=	-lcitro2d -lcitro3d -lctru -lm

all: $(TARGET).3dsx $(TARGET).cia

$(TARGET).3dsx: $(TARGET).elf

$(TARGET).cia: $(TARGET).elf
	smdhtool --create "PSZ Studio" "Encrypted Archive Manager" "Manus AI" ../../assets/3ds/icon.png icon.smdh
	bannertool makebanner -i ../../assets/3ds/banner.png -o banner.bnr
	makerom -f cia -o $(TARGET).cia -target t -exheader exheader.bin -Code $(TARGET).elf -icon icon.smdh -banner banner.bnr -rsf 3ds.rsf || \
	makerom -f cia -o $(TARGET).cia -exefslogo -icon icon.smdh -banner banner.bnr -code $(TARGET).elf

%.elf:
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	@echo clean ...
	@rm -fr build $(TARGET).3dsx $(TARGET).cia $(TARGET).elf icon.smdh banner.bnr
