API=../../api
CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
CFLAGS=-DNDEBUG -ggdb3 -I$(API) -Os -mcpu=cortex-m7 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard -fno-common -fdata-sections -ffunction-sections -fno-exceptions
LDFLAGS=-Wl,-L$(API) -Wl,--gc-sections -Wl,--entry=entrypoint --specs=nosys.specs -nostartfiles -Wl,-Ur -lapi -lm

NES_ROM = epsilon/2048.nes

CFLAGS += -I./ -Ines/ -Isndhrdw/ -Ilibsnss/ -Icpu/ -Iepsilon/

OBJS = bitmap.o \
	config.o \
	cpu/dis6502.o \
	cpu/nes6502.o \
	event.o \
	libsnss/libsnss.o \
	log.o \
	pcx.o \
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
	nes/nesinput.o \
	nes/nes_mmc.o \
	nes/nes_pal.o \
	nes/nes_ppu.o \
	nes/nes_rom.o \
	nes/nesstate.o \
	sndhrdw/fds_snd.o \
	sndhrdw/mmc5_snd.o \
	sndhrdw/nes_apu.o \
	sndhrdw/vrcvisnd.o \
	nofrendo.o \
	epsilon/lz4.o \
	epsilon/stubs.o \
	epsilon/osd.o \
	epsilon/rom.o \
	epsilon/statefile_wrapper.o \
	epsilon/video_audio.o \
	epsilon/main.o

app.elf: $(OBJS)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

epsilon/rom.c: $(NES_ROM) FORCE
	echo "const unsigned char _rom_data[] = {" > $@
	cat "$(NES_ROM)" | xxd -i >> $@
	echo "};" >> $@

FORCE:

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) app.elf