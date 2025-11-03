#ifndef INPUT_INPUT_H_
#define INPUT_INPUT_H_

#include <stdbool.h>

// Input state structure
typedef struct {
  int dir_x;
  int dir_y;
  bool select_pressed;
  bool confirm_pressed;
} InputState;

// Function prototypes
void INPUT_init(void);
InputState INPUT_poll(void);

#endif /* INPUT_INPUT_H_ */
