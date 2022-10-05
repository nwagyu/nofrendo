// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <extapp_api.h>
#undef false
#undef true
#undef bool
#include <math.h>
#include <string.h>
#include <noftypes.h>
#include <bitmap.h>
#include <nofconfig.h>
#include <event.h>
#include <gui.h>
#include <log.h>
#include <nes.h>
#include <nes_pal.h>
#include <nesinput.h>
#include <osd.h>
#include <stdint.h>

#define  DEFAULT_SAMPLERATE   22050
#define  DEFAULT_FRAGSIZE     128

#define  DEFAULT_WIDTH        256
#define  DEFAULT_HEIGHT       NES_VISIBLE_HEIGHT

int timerfreq = 50;

//Seemingly, this will be called only once. Should call func with a freq of frequency,
int osd_installtimer(int frequency, void *func, int funcsize, void *counter, int countersize)
{
	timerfreq = frequency;
	return 0;
}


int osd_nofrendo_ticks(void) {
	return extapp_millis() / (1000 / timerfreq);
}

/*
** Audio
*/
void osd_setsound(void (*playfunc)(void *buffer, int length)) {
	//Indicates we should call playfunc() to get more data.
}

void osd_getsoundinfo(sndinfo_t *info) {
	info->sample_rate = DEFAULT_SAMPLERATE;
	info->bps = 8;
}

/*
** Video
*/

static int init(int width, int height);
static void shutdown(void);
static int set_mode(int width, int height);
static void set_palette(rgb_t *pal);
static void clear(uint8 color);
static bitmap_t *lock_write(void);
static void free_write(int num_dirties, rect_t *dirty_rects);
static void custom_blit(bitmap_t *bmp, int num_dirties, rect_t *dirty_rects);
static char fb[1]; //dummy

viddriver_t pkspDriver =
{
   "video",       /* name */
   init,          /* init */
   shutdown,      /* shutdown */
   set_mode,      /* set_mode */
   set_palette,   /* set_palette */
   clear,         /* clear */
   lock_write,    /* lock_write */
   free_write,    /* free_write */
   custom_blit,   /* custom_blit */
   false          /* invalidate flag */
};

bitmap_t *myBitmap;

void osd_getvideoinfo(vidinfo_t *info) {
   info->default_width = DEFAULT_WIDTH;
   info->default_height = DEFAULT_HEIGHT;
   info->driver = &pkspDriver;
}

/* flip between full screen and windowed */
void osd_togglefullscreen(int code) {
}

/* initialise video */
static int init(int width, int height) {
	return 0;
}

static void shutdown(void) {
}

/* set a video mode */
static int set_mode(int width, int height) {
	return 0;
}

static uint16 myPalette[256];

/* copy nes palette over to hardware */
static void set_palette(rgb_t *pal) {
   for (int i = 0; i < 256; i++) {
      myPalette[i] = (pal[i].b>>3)+((pal[i].g>>2)<<5)+((pal[i].r>>3)<<11);
   }
}

void vid_setpalette(rgb_t *pal) {
   set_palette(pal);
}

/* clear all frames to a particular color */
static void clear(uint8 color) {
}

/* acquire the directbuffer for writing */
static bitmap_t *lock_write(void) {
   myBitmap = bmp_createhw((uint8*)fb, DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_WIDTH*2);
   return myBitmap;
}

/* release the resource */
static void free_write(int num_dirties, rect_t *dirty_rects) {
   bmp_destroy(&myBitmap);
}

static void custom_blit(bitmap_t *bmp, int num_dirties, rect_t *dirty_rects) {
	uint16_t line[bmp->width];
	int xoffset = (LCD_WIDTH - bmp->width) / 2;
	int yoffset = (LCD_HEIGHT - bmp->height) / 2;

	for(int y=0; y<bmp->height; y++) {
		for(int x=0; x<bmp->width; x++) {
			line[x] = myPalette[bmp->line[y][x]];
		}
		extapp_pushRect(xoffset, y+yoffset, bmp->width, 1, line);
	}
}

static int do_loadstate=0;

void ppu_scanline_blit(uint8_t *bmp, int scanline, bool draw_flag) {
	uint16_t line[NES_SCREEN_WIDTH];
	const int xoffset = (LCD_WIDTH - NES_SCREEN_WIDTH) / 2;
	const int yoffset = (LCD_HEIGHT - NES_SCREEN_HEIGHT) / 2;
	bmp += 8;
	if(draw_flag && !(scanline < 0 || scanline >= LCD_HEIGHT) && !do_loadstate) {
		for(int x=0; x<NES_SCREEN_WIDTH; x++) {
			line[x] = myPalette[*bmp++];
		}
		extapp_pushRect(xoffset, scanline+yoffset, NES_SCREEN_WIDTH, 1, line);
	}
}

/*
** Input
*/
void osd_queue_loadstate() {
	do_loadstate=5;
}

void osd_getinput(void) {
	const int events[9]={
    event_joypad1_left,
    event_joypad1_up,
    event_joypad1_down,
    event_joypad1_right,
    event_joypad1_b,
    event_joypad1_a,
    event_joypad1_select,
    event_joypad1_start,
    event_joypad1_start,
  };
  static uint64_t old_keys_pressed = 0xffffffffffffffff;

  uint64_t cur_keys_pressed = extapp_scanKeyboard() & ~SCANCODE_EXE;
  uint64_t chg_keys_pressed = cur_keys_pressed ^ old_keys_pressed;
  old_keys_pressed = cur_keys_pressed;

	//do_loadstate is set to a certain number on bootup, because for some reason loading the state
	//doesn't work directly after boot-up. This causes it to wait for a few frames.
	if (do_loadstate>0) do_loadstate--;
	if (do_loadstate==1) {
		event_get(event_state_slot_0)(INP_STATE_MAKE);
		event_get(event_state_load)(INP_STATE_MAKE);
	}

  if(cur_keys_pressed & (1<<17)) {
    event_get(event_hard_reset)(INP_STATE_MAKE);
  } else if(cur_keys_pressed > 511) {
		event_get(event_state_slot_0)(INP_STATE_MAKE);
		event_get(event_state_save)(INP_STATE_MAKE);
    event_get(event_quit)(INP_STATE_MAKE);
  }
  for (int i=0; i<9; i++) {
    if (chg_keys_pressed & 1) {
      event_get(events[i])((cur_keys_pressed & 1) ? INP_STATE_MAKE : INP_STATE_BREAK);
    }
    chg_keys_pressed >>= 1;
    cur_keys_pressed >>= 1;
  }
	extapp_msleep(0);
}

void osd_getmouse(int *x, int *y, int *button) {
}

/* this is at the bottom, to eliminate warnings */
void osd_shutdown() {
}

static int logprint(const char *string) {
   return printf("%s", string);
}

/*
** Startup
*/
int osd_init()
{
	return 0;
}
