.SUFFIXES:

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment.")
endif

include $(DEVKITPRO)/wut/share/wut_rules

TARGET := psz_wiiu
BUILD := build
SOURCES := . ../../src
INCLUDES := ../../include

APP_NAME := PSZ Studio
APP_SHORTNAME := PSZ
APP_AUTHOR := SlabyLol
ICON := ../../assets/wiiu/icon.png
TV_SPLASH := ../../assets/wiiu/banner.png
DRC_SPLASH := ../../assets/wiiu/banner.png

CFLAGS := -g -Wall -O2 -ffunction-sections $(MACHDEP) $(INCLUDE) -I../../include -D__WIIU__ -D__WUT__
CXXFLAGS := $(CFLAGS)
ASFLAGS := -g $(ARCH)
LDFLAGS := -g $(ARCH) $(RPXSPECS) -Wl,-Map,$(notdir $*.map)
LIBS := -lwhb -lwut
LIBDIRS := $(PORTLIBS) $(WUT_ROOT)

ifneq ($(BUILD),$(notdir $(CURDIR)))
export OUTPUT := $(CURDIR)/$(TARGET)
export TOPDIR := $(CURDIR)
export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR := $(CURDIR)/$(BUILD)
CFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
export OFILES := $(CFILES:.c=.o)
export INCLUDE := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) $(foreach dir,$(LIBDIRS),-I$(dir)/include) -I$(CURDIR)/$(BUILD)
export LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)
export APP_ICON := $(TOPDIR)/$(ICON)
export APP_TV_SPLASH := $(TOPDIR)/$(TV_SPLASH)
export APP_DRC_SPLASH := $(TOPDIR)/$(DRC_SPLASH)

.PHONY: all clean
all: $(BUILD)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@rm -fr $(BUILD) $(TARGET).elf $(TARGET).rpx $(TARGET).wuhb
else
DEPENDS := $(OFILES:.o=.d)
.PHONY: all
all: $(OUTPUT).wuhb
$(OUTPUT).wuhb: $(OUTPUT).rpx
$(OUTPUT).rpx: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)
-include $(DEPENDS)
endif
