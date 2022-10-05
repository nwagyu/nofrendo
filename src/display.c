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

#include <eadk.h>
#undef false
#undef true
#undef bool
#include <osd.h>
#include <bitmap.h>
#include <nes.h>

#define  DEFAULT_WIDTH        256
#define  DEFAULT_HEIGHT       NES_VISIBLE_HEIGHT

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
	int xoffset = (EADK_SCREEN_WIDTH - bmp->width) / 2;
	int yoffset = (EADK_SCREEN_HEIGTH - bmp->height) / 2;

	for(int y=0; y<bmp->height; y++) {
		for(int x=0; x<bmp->width; x++) {
			line[x] = myPalette[bmp->line[y][x]];
		}
		eadk_display_push_rect((eadk_rect_t){xoffset, y+yoffset, bmp->width, 1}, line);
	}
}

void ppu_scanline_blit(uint8_t *bmp, int scanline, bool draw_flag) {
	uint16_t line[NES_SCREEN_WIDTH];
	const int xoffset = (EADK_SCREEN_WIDTH - NES_SCREEN_WIDTH) / 2;
	const int yoffset = (EADK_SCREEN_HEIGTH - NES_SCREEN_HEIGHT) / 2;
	bmp += 8;
	if(draw_flag && !(scanline < 0 || scanline >= EADK_SCREEN_HEIGTH)) {
		for(int x=0; x<NES_SCREEN_WIDTH; x++) {
			line[x] = myPalette[*bmp++];
		}
		eadk_display_push_rect((eadk_rect_t){xoffset, scanline+yoffset, NES_SCREEN_WIDTH, 1}, line);
	}
}
