#include "display.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include "hardware/dma.h"

uint16_t vram[SCREEN_W * SCREEN_H];
int dma_channel;

void write_cmd(uint8_t cmd)
{
  gpio_put(CS_PIN, 0);
  gpio_put(DC_PIN, 0); 
  spi_write_blocking(SPI_PORT, &cmd, 1);
  gpio_put(CS_PIN, 1);
}

void write_data(uint8_t data)
{
  gpio_put(CS_PIN, 0);
  gpio_put(DC_PIN, 1); 
  spi_write_blocking(SPI_PORT, &data, 1);
  gpio_put(CS_PIN, 1);
}

bool init_display(void)
{
  spi_init(SPI_PORT, 50000000); // 50 mhz
  gpio_set_function(SCK_PIN, GPIO_FUNC_SPI);
  gpio_set_function(MOSI_PIN, GPIO_FUNC_SPI);

  gpio_init(CS_PIN);
  gpio_set_dir(CS_PIN, GPIO_OUT);
  gpio_put(CS_PIN, 1);
  gpio_init(DC_PIN);
  gpio_set_dir(DC_PIN, GPIO_OUT);
  gpio_put(DC_PIN, 0);
  gpio_init(RESET_PIN);
  gpio_set_dir(RESET_PIN, GPIO_OUT);
  gpio_put(RESET_PIN, 1);

  gpio_put(RESET_PIN, 0);
  sleep_ms(50);
  gpio_put(RESET_PIN, 1);
  sleep_ms(150);

  write_cmd(0x01);
  sleep_ms(150);

  write_cmd(0x11);
  sleep_ms(150);

  write_cmd(0x3A);
  write_data(0x55);

  dma_channel = dma_claim_unused_channel(true);
  dma_channel_config c = dma_channel_get_default_config(dma_channel);
  
  channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
  channel_config_set_dreq(&c, spi_get_dreq(SPI_PORT, true));
  channel_config_set_read_increment(&c, true);
  channel_config_set_write_increment(&c, false);

  dma_channel_configure(dma_channel, &c, &spi_get_hw(SPI_PORT)->dr, vram, SCREEN_W * SCREEN_H * 2, false);

  write_cmd(0x36);
  write_data(0x28); // portrait: 0x48, landscape: 0x28, inverse portrait: 0x88, inverse landscape: 0xE8

  write_cmd(0x29);
  sleep_ms(50);

  return true;
}

void update_display()
{
  dma_channel_wait_for_finish_blocking(dma_channel);
  
  gpio_put(CS_PIN, 0);

  write_cmd(0x2A);
  write_data(0x00);
  write_data(0x00);
  write_data(0x01);
  write_data(0x3F);

  write_cmd(0x2B);
  write_data(0x00);
  write_data(0x00);
  write_data(0x00);
  write_data(0xEF);

  write_cmd(0x2C);

  gpio_put(CS_PIN, 0);
  gpio_put(DC_PIN, 1);

  dma_channel_set_read_addr(dma_channel, vram, true);
}
