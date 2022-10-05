#include <osd.h>

#define DEFAULT_SAMPLERATE 22050

void osd_setsound(void (*playfunc)(void *buffer, int length)) {
}

void osd_getsoundinfo(sndinfo_t *info) {
  info->sample_rate = DEFAULT_SAMPLERATE;
  info->bps = 8;
}
