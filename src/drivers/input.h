#ifndef INPUT_H
#define INPUT_H

#define BTN_UP    3
#define BTN_DOWN  4
#define BTN_LEFT  5
#define BTN_RIGHT 6
#define BTN_A     16
#define BTN_B     17

#define MAX_BUTTONS sizeof(buttons) / sizeof(buttons[0])

#include <stdint.h>
#include <stdbool.h>

#include "hardware/gpio.h"

void init_input(void);
bool is_pressed(uint8_t btn);

#endif
