#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "pico/time.h"

// local headers
#include "display/display.h"
#include "display/renderer.h"
#include "utils/color.h"
#include "utils/type.h"
#include "drivers/input.h"
#include "models/test_model.h"

extern int dma_channel;

static uint16_t color_arr[] = { CL_WHITE, CL_RED, CL_GREEN, CL_BLUE, CL_YELLOW, CL_CYAN, CL_MAGENTA, CL_GRAY, CL_ORANGE };

static uint8_t color_idx = 0;

int main()
{
  stdio_init_all();
  while(!init_display()) {}
  init_input();

  sleep_ms(200);

  camera_t c = {
    .position = {0, 0, 0},
    .rotation = {0, 0, 0},
    .focal_length = 150.0f
  };

  static object_t o = {
    .position = {55, 0, 150},
    .scale = 8.0f,
    .mesh = &test_model_mesh,
    .color = CL_BLUE 
  };

  static object_t o1 = {
    .position = {-55, 0, 150},
    .scale = 8.0f,
    .mesh = &test_model_mesh,
    .color = CL_BLUE
  };

  uint32_t last_change = to_ms_since_boot(get_absolute_time());

  while(true)
  {
    dma_channel_wait_for_finish_blocking(dma_channel);

    uint32_t current = to_ms_since_boot(get_absolute_time());

    if(current - last_change >= 500)
    {
      color_idx = (color_idx + 1) % 9;
      o.color = color_arr[color_idx];
      o1.color = color_arr[color_idx];
      last_change = current;
    }
    
    fill_screen(CL_BLACK);

    vec2_t projected[o.mesh->vertex_count];
    bool visible[o.mesh->vertex_count];

    render_faces(&o, &c, projected, visible);
    render_wireframe(&o1, &c, projected, visible);

    o.rotation.x += 0.05f;
    o.rotation.y += 0.05f;
    o.rotation.z += 0.05f;

    o1.rotation.x += 0.05f;
    o1.rotation.y += 0.05f;
    o1.rotation.z += 0.05f;
    
    update_display();
  }

  return 0;
}
