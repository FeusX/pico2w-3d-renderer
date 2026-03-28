#include "input.h"

void init_input(void)
{
  const uint8_t buttons[] = {BTN_UP, BTN_DOWN, BTN_LEFT, BTN_RIGHT, BTN_A, BTN_B};
  for(int i = 0; i < MAX_BUTTONS; i++)
  {
    gpio_init(buttons[i]);
    gpio_set_dir(buttons[i], GPIO_IN);
    gpio_pull_up(buttons[i]);
  }
}

bool is_pressed(uint8_t btn)
{ return !gpio_get(btn); }
