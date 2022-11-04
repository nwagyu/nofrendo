Q ?= @
CC = arm-none-eabi-gcc
NWLINK = npx --yes -- nwlink@0.0.15
LINK_GC = 1
LTO = 1

CFLAGS += -Os -DNDEBUG
CFLAGS += $(shell $(NWLINK) eadk-cflags)
LDFLAGS = -Wl,--relocatable
LDFLAGS += -nostartfiles
LDFLAGS += --specs=nano.specs

ifeq ($(LINK_GC),1)
CFLAGS += -fdata-sections -ffunction-sections
LDFLAGS += -Wl,-e,main -Wl,-u,eadk_app_name -Wl,-u,eadk_app_icon -Wl,-u,eadk_api_level
LDFLAGS += -Wl,--gc-sections
endif

ifeq ($(LTO),1)
CFLAGS += -flto -fno-fat-lto-objects
CFLAGS += -fwhole-program
CFLAGS += -fvisibility=internal
LDFLAGS += -flinker-output=nolto-rel
endif

CFLAGS += -Isrc -Isrc/nofrendo -Isrc/nofrendo/nes -Isrc/nofrendo/libsnss -Isrc/nofrendo/cpu

objs = $(addprefix output/nofrendo/,\
  bitmap.o \
  config.o \
  cpu/dis6502.o \
  cpu/nes6502.o \
  event.o \
  libsnss/libsnss.o \
  log.o \
  mappers/map000.o \
  mappers/map001.o \
  mappers/map002.o \
  mappers/map003.o \
  mappers/map004.o \
  mappers/map005.o \
  mappers/map007.o \
  mappers/map008.o \
  mappers/map009.o \
  mappers/map011.o \
  mappers/map015.o \
  mappers/map016.o \
  mappers/map018.o \
  mappers/map019.o \
  mappers/map024.o \
  mappers/map032.o \
  mappers/map033.o \
  mappers/map034.o \
  mappers/map040.o \
  mappers/map041.o \
  mappers/map042.o \
  mappers/map046.o \
  mappers/map050.o \
  mappers/map064.o \
  mappers/map065.o \
  mappers/map066.o \
  mappers/map069.o \
  mappers/map070.o \
  mappers/map073.o \
  mappers/map075.o \
  mappers/map078.o \
  mappers/map079.o \
  mappers/map085.o \
  mappers/map087.o \
  mappers/map093.o \
  mappers/map094.o \
  mappers/map099.o \
  mappers/map160.o \
  mappers/map225.o \
  mappers/map229.o \
  mappers/map231.o \
  mappers/mapvrc.o \
  memguard.o \
  nes/mmclist.o \
  nes/nes.o \
  nes/nes_mmc.o \
  nes/nes_pal.o \
  nes/nes_ppu.o \
  nes/nes_rom.o \
  nes/nesinput.o \
  nes/nesstate.o \
  nofrendo.o \
  pcx.o \
)

ifeq ($(AUDIO),1)
objs = $(addprefix output/nofrendo/,\
  sndhrdw/fds_snd.o \
  sndhrdw/mmc5_snd.o \
  sndhrdw/nes_apu.o \
  sndhrdw/vrcvisnd.o \
)
CFLAGS += -DAUDIO=1 -Isrc/nofrendo/sndhrdw
endif

objs += $(addprefix output/, \
  icon.o \
  timing.o \
  display.o \
  sound.o \
  keyboard.o \
  main.o \
  osd.o \
  statefile_wrapper.o \
  stubs.o \
)

.PHONY: build
build: output/nofrendo.nwa

.PHONY: check
check: output/nofrendo.bin

output/nofrendo.bin: output/nofrendo.nwa
	@echo "NWLINK  $@"
	$Q $(NWLINK) nwa-bin -d src/2048.nes $< $@

output/nofrendo.nwa: $(objs)
	@echo "LD      $@"
	$Q $(CC) $(CFLAGS) $(LDFLAGS) $^ -lm -o $@

output/%.o: src/%.c
	@mkdir -p $(dir $@)
	@echo "CC      $^"
	$Q $(CC) $(CFLAGS) -c $^ -o $@

output/icon.o: src/icon.png
	@echo "ICON    $<"
	$(Q) $(NWLINK) png-icon-o $< $@

.PHONY: clean
clean:
	@echo "CLEAN"
	$Q rm -rf output
