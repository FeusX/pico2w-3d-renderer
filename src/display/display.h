#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>

#define SPI_PORT  spi0
#define SCK_PIN   18
#define MOSI_PIN  19
#define CS_PIN    22
#define DC_PIN    27
#define RESET_PIN 28

#define SCREEN_W 320
#define SCREEN_H 240

extern uint16_t vram[SCREEN_W * SCREEN_H];

bool init_display(void);
void update_display(void);
void write_cmd(uint8_t cmd);
void write_data(uint8_t data);

#endif
