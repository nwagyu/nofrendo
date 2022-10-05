#include <stdint.h>
#include <gui.h>
#include <vid_drv.h>

void gui_tick(int ticks) {}
void gui_setrefresh(int frequency) {}

void gui_sendmsg(int color, char *format, ...) {}

int gui_init(void) {
  return 0;
}
void gui_shutdown(void) {}

void gui_frame(bool draw) {}

void gui_togglefps(void) {}
void gui_togglegui(void) {}
void gui_togglewave(void) {}
void gui_togglepattern(void) {}
void gui_toggleoam(void) {}

void gui_decpatterncol(void) {}
void gui_incpatterncol(void) {}

void gui_savesnap(void) {}
void gui_togglesprites(void) {}
void gui_togglefs(void) {}
void gui_displayinfo() {}
void gui_toggle_chan(int chan) {}
void gui_setfilter(int filter_type) {}

rgb_t gui_pal[GUI_TOTALCOLORS] =
{
   { 0x00, 0x00, 0x00 }, /* black      */
   { 0x3F, 0x3F, 0x3F }, /* dark gray  */
   { 0x7F, 0x7F, 0x7F }, /* gray       */
   { 0xBF, 0xBF, 0xBF }, /* light gray */
   { 0xFF, 0xFF, 0xFF }, /* white      */
   { 0xFF, 0x00, 0x00 }, /* red        */
   { 0x00, 0xFF, 0x00 }, /* green      */
   { 0x00, 0x00, 0xFF }, /* blue       */
   { 0xFF, 0xFF, 0x00 }, /* yellow     */
   { 0xFF, 0xAF, 0x00 }, /* orange     */
   { 0xFF, 0x00, 0xFF }, /* purple     */
   { 0x3F, 0x7F, 0x7F }, /* teal       */
   { 0x00, 0x2A, 0x00 }, /* dk. green  */
   { 0x00, 0x00, 0x3F }  /* dark blue  */
};

int  vid_init(int width, int height, viddriver_t *osd_driver) {
  return 0;
}

void vid_flush(void) {
}

int  vid_setmode(int width, int height) {
  return 0;
}

void vid_shutdown(void) {
}
