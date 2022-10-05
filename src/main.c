#include <eadk.h>
#include <nofrendo.h>
#include <stddef.h>
#include <string.h>

const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "NES";
const uint32_t eadk_api_level  __attribute__((section(".rodata.eadk_api_level"))) = 0;

// Newlib's implementation of rand allocates and raises
// It's only used to trash memory anyway
int rand() {
  return 0;
}

extern void osd_queue_loadstate();

const char *osd_getromdata(const char *name) {
  return (char*)eadk_external_data;
}

void osd_unloadromdata() {
}

static void waitForKeyReleasedTimeout(int timeout) {
  while(eadk_keyboard_scan() && timeout > 0) {
    eadk_timing_msleep(10);
    timeout -= 10;
  }
}

static void waitForKeyReleased() {
  while(eadk_keyboard_scan()) {
    eadk_timing_msleep(10);
  }
}

static void waitForKeyPressed() {
  while(!eadk_keyboard_scan()) {
    eadk_timing_msleep(10);
  }
}

extern char *osd_newextension(char *string, char *ext);

int main(int argc, char * argv[]) {
  eadk_display_push_rect_uniform(eadk_screen_rect, eadk_color_black);
  nofrendo_main(0, NULL);
}
