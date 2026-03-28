#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

#define RGB565(r, g, b) (uint16_t)((((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)) << 8 | (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)) >> 8)

#define CL_WHITE    0xFFFF
#define CL_BLACK    0x0000
#define CL_RED      0x00F8
#define CL_GREEN    0xE007
#define CL_BLUE     0x1F00
#define CL_YELLOW   0xE0FF
#define CL_CYAN     0xFF07
#define CL_MAGENTA  0x1FF8
#define CL_GRAY     0x1084
#define CL_ORANGE   0x20FD

#define CL_DARK_RED    0x0080
#define CL_DARK_GREEN  0x0004
#define CL_DARK_BLUE   0x1000
#define CL_DARK_GRAY   0x0842

#endif