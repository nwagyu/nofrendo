#include <eadk.h>
#undef false
#undef true
#undef bool
#include <osd.h>
#include <event.h>
#include <nesinput.h>

void osd_getinput(void) {
  typedef struct {
    eadk_key_t key;
    int event;
  } key_event_mapping;
  const key_event_mapping key_to_events[] = {
    {eadk_key_left, event_joypad1_left},
    {eadk_key_up, event_joypad1_up},
    {eadk_key_down, event_joypad1_down},
    {eadk_key_right, event_joypad1_right},
    {eadk_key_ok, event_joypad1_b},
    {eadk_key_back, event_joypad1_a},
    {eadk_key_shift, event_joypad1_select},
    {eadk_key_backspace, event_joypad1_start},
  };

  static uint64_t old_keyboard_state = 0xffffffffffffffff;
  uint64_t current_keyboard_state = eadk_keyboard_scan();

  for (int i=0; i<sizeof(key_to_events)/sizeof(key_to_events[0]); i++) {
    bool wasUp = eadk_keyboard_key_down(old_keyboard_state, key_to_events[i].key);
    bool isUp = eadk_keyboard_key_down(current_keyboard_state, key_to_events[i].key);
    if (isUp != wasUp) {
      event_t evt = event_get(key_to_events[i].event);
      evt(isUp ? INP_STATE_MAKE : INP_STATE_BREAK);
    }
  }

  old_keyboard_state = current_keyboard_state;
}
