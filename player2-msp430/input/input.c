#include <input/input.h>
#include <drivers/joystick.h>
#include <drivers/switch.h>
#include <hal/hal_adc.h>

#define JOYSTICK_THRESHOLD 85

// Static variables for state tracking
static int prev_dir_x = 0;
static int prev_dir_y = 0;
static bool first_read = true;
static bool prev_s1_state = false;
static bool prev_s2_state = false;

void INPUT_init(void) {
  prev_dir_x = 0;
  prev_dir_y = 0;
  first_read = true;
  prev_s1_state = false;
  prev_s2_state = false;
}

InputState INPUT_poll(void) {
  InputState input = {0};

  // Read joystick
  float joystick_x = JOYSTICK_get_x();
  float joystick_y = JOYSTICK_get_y();
  int dir_x = 0;
  int dir_y = 0;

  if (joystick_y > JOYSTICK_THRESHOLD)
    dir_y = -1;
  else if (joystick_y < -JOYSTICK_THRESHOLD)
    dir_y = 1;
  else if (joystick_x > JOYSTICK_THRESHOLD)
    dir_x = 1;
  else if (joystick_x < -JOYSTICK_THRESHOLD)
    dir_x = -1;

  // Initialize prev values on first read to avoid initial movement
  if (first_read) {
    prev_dir_x = dir_x;
    prev_dir_y = dir_y;
    first_read = false;
  }

  // Only report direction on a new direction (edge detection)
  if ((dir_x != 0 || dir_y != 0) &&
      (dir_x != prev_dir_x || dir_y != prev_dir_y)) {
    input.dir_x = dir_x;
    input.dir_y = dir_y;
  }

  prev_dir_x = dir_x;
  prev_dir_y = dir_y;

  // Read buttons (edge detection - only trigger on press, not hold)
  bool s1_state = SWITCH_get_edumkii_S1();
  bool s2_state = SWITCH_get_edumkii_S2();

  input.select_pressed = (s1_state && !prev_s1_state);
  input.confirm_pressed = (s2_state && !prev_s2_state);

  prev_s1_state = s1_state;
  prev_s2_state = s2_state;

  return input;
}
