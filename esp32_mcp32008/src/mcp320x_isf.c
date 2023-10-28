#include "mcp320x_isf.h"

uint16_t mcp_read(spi_device_handle_t spi_port, mcp320x_read_mode_t read_mode,
                  mcp320x_channel_t channel, uint16_t sample_count) {
  uint16_t value = 0;
  uint32_t sum = 0;
  char data[3];
  // spi_transaction_t transaction = {
  //     .flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA, .length = 24};

  spi_transaction_t trans_desc;
  trans_desc.addr = 0;
  trans_desc.cmd = 0;
  trans_desc.flags = 0;
  trans_desc.length = 3 * 8;
  trans_desc.rxlength = 0;
  trans_desc.tx_buffer = data;
  trans_desc.rx_buffer = data;

  data[0] = (uint8_t)((1 << 2) | (read_mode << 1) | ((channel & 4) >> 2));
  data[1] = (uint8_t)(channel << 6);
  data[2] = 0;

  for (uint16_t i = 0; i < sample_count; i++) {
    ESP_ERROR_CHECK(spi_device_polling_transmit(spi_port, &trans_desc));
    const uint16_t first_part = trans_desc.rx_data[1];
    const uint16_t second_part = trans_desc.rx_data[2];

    sum += ((first_part & 15) << 8) | second_part;
    vTaskDelay(1);
  }
  ESP_ERROR_CHECK(spi_bus_remove_device(spi_port));
  ESP_ERROR_CHECK(spi_bus_free(SPI2_HOST));
  value = (uint16_t)(sum / sample_count);
  return value;
}