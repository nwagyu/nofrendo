#include <extapp_api.h>
#include <nofrendo.h>
#include <stddef.h>
#include <string.h>

#define FILENAME_LENGHT_MAX 512

extern void osd_queue_loadstate();

const char *osd_getromdata(const char *name) {
	return extapp_fileRead(name, NULL, EXTAPP_FLASH_FILE_SYSTEM);
}

void osd_unloadromdata() {
}


static void waitForKeyReleasedTimeout(int timeout) {
	while(extapp_scanKeyboard() && timeout > 0) {
		extapp_msleep(10);
		timeout -= 10;
	}
}

static void waitForKeyReleased() {
	while(extapp_scanKeyboard()) {
		extapp_msleep(10);
	}
}

static void waitForKeyPressed() {
	while(!extapp_scanKeyboard()) {
		extapp_msleep(10);
	}
}

char ** remove(char ** first, char ** last) {
  char ** result = first;
  while (first != last) {
    if (!(*first == NULL)) {
      *result = *first;
      ++result;
    }
    ++first;
  }
  return result;
}

extern char *osd_newextension(char *string, char *ext);

void drawRomList(char **filenames, int nb, int selected_rom) {
	char name_buffer[FILENAME_LENGHT_MAX];
	for(int i = 0; i < nb; i++) {
		strncpy(name_buffer, filenames[i], FILENAME_LENGHT_MAX);
		name_buffer[26] = '\0';
		extapp_drawTextLarge(name_buffer, 0, (i+1)*20, selected_rom == i ? 0b1111100000000000 : 65535, 0, false);
		strncpy(name_buffer, filenames[i], FILENAME_LENGHT_MAX);
		if(extapp_fileExists(osd_newextension(name_buffer, ".ss0"), EXTAPP_RAM_FILE_SYSTEM)) {
			extapp_drawTextLarge("Saved", LCD_WIDTH - 50, (i+1)*20, selected_rom == i ? 0b1111100000000000 : 65535, 0, false);
		} else {
			extapp_drawTextLarge("     ", LCD_WIDTH - 50, (i+1)*20, selected_rom == i ? 0b1111100000000000 : 65535, 0, false);
		}
	}
}

static const char * select_rom() {
	const int max_roms = 10;
	char * filenames[max_roms];
	int selected_rom = 0;
	char name_buffer[FILENAME_LENGHT_MAX];

	waitForKeyReleased();

	extapp_pushRectUniform(0, 0, LCD_WIDTH, LCD_HEIGHT, 0);
	extapp_drawTextLarge("          Select a ROM          ", 0, 0, 65535, 14567, false);

	int nb = extapp_fileListWithExtension(filenames, max_roms, "", EXTAPP_FLASH_FILE_SYSTEM);

	size_t len;
	for(int i = 0; i < nb; i++) {
		const char * data = extapp_fileRead(filenames[i], &len, EXTAPP_FLASH_FILE_SYSTEM);
		if(len < 4 || data[0] != 'N' || data[1] != 'E' || data[2] != 'S' || data[3] != 0x1a) {
			filenames[i] = NULL;
		}
	}
	nb = remove(filenames, filenames + nb) - filenames;

	if(nb == 0) {
		extapp_drawTextLarge("          No ROM found          ", 0, 120, 65535, 0, false);
		extapp_msleep(10);
		waitForKeyPressed();
		return NULL;
	} else {
		drawRomList(filenames, nb, selected_rom);
		for(;;) {
			extapp_msleep(10);
			uint64_t scancode = extapp_scanKeyboard();
			if(scancode & SCANCODE_Down) {
				selected_rom = (selected_rom + 1) % nb;
				drawRomList(filenames, nb, selected_rom);
				waitForKeyReleasedTimeout(200);
			} else if(scancode & SCANCODE_Up) {
				selected_rom = (selected_rom - 1) % nb;
				if(selected_rom < 0) {
					selected_rom = nb + selected_rom;
				}
				drawRomList(filenames, nb, selected_rom);
				waitForKeyReleasedTimeout(200);
			} else if(scancode & (SCANCODE_OK | SCANCODE_EXE))  {
				break;
			} else if(scancode & SCANCODE_Backspace) {
				strncpy(name_buffer, filenames[selected_rom], FILENAME_LENGHT_MAX);
				extapp_fileErase(osd_newextension(name_buffer, ".ss0"), EXTAPP_RAM_FILE_SYSTEM);
				drawRomList(filenames, nb, selected_rom);
				waitForKeyReleased();
			} else if(scancode & SCANCODE_Back) {
				return NULL;
			}
		}
		extapp_pushRectUniform(0, 0, LCD_WIDTH, LCD_HEIGHT, 0);
		return filenames[selected_rom];
	}
}

void extapp_main() {
  //Create commandline args
  const char *args[3];
  args[0]="nofrendo";
  args[1]=select_rom();
  args[2]=NULL;

	if(args[1]) {
		osd_queue_loadstate();
		nofrendo_main(2, args);
	}
}
