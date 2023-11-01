#include "mcp320x_isf.h"
#include "spi_driver.h"

#include "esp_task_wdt.h"
#include "soc/timer_group_reg.h"
#include "soc/timer_group_struct.h"

// void feedTheDog() {
//   // feed dog 0
//   TIMERG0.wdtwprotect = TIMG_WDT_WKEY_VALUE; // write enable
//   TIMERG0.wdtfeed = 1;                       // feed dog
//   TIMERG0.wdtwprotect = 0;                   // write protect
//   // feed dog 1
//   TIMERG1.wdtwprotect = TIMG_WDT_WKEY_VALUE; // write enable
//   TIMERG1.wdtfeed = 1;                       // feed dog
//   TIMERG1.wdtwprotect = 0;                   // write protect
// }

uint16_t mcp_read(SPIDriver *spi, mcp320x_read_mode_t read_mode,
                  mcp320x_channel_t channel, uint16_t sample_count) {

  esp_err_t ret;
  uint16_t value = 0;
  uint32_t sum = 0;
  uint8_t tx_data[3];
  uint8_t rx_data[3];

  tx_data[0] = (uint8_t)((1 << 2) | (read_mode << 1) | ((channel & 4) >> 2));
  tx_data[1] = (uint8_t)(channel << 6);
  tx_data[2] = 0;

  for (uint16_t i = 0; i < sample_count; i++) {
    ret = spi_driver_transfer(spi, tx_data, sizeof(tx_data), rx_data,
                              sizeof(rx_data));
    const uint16_t first_part = rx_data[1];
    const uint16_t second_part = rx_data[2];

    // ESP_LOGI("rx", "rx=0x%02X 0x%02X", rx_data[1], rx_data[2]);
    sum += ((first_part & 15) << 8) | second_part;
    // esp_task_wdt_reset();
  }
  value = (uint16_t)(sum / sample_count);
  return value;
}

double mcp_read_voltage(SPIDriver *spi, mcp320x_read_mode_t read_mode,
                        mcp320x_channel_t channel, uint16_t sample_count) {

  uint16_t value = 0;

  value = mcp_read(spi, read_mode, channel, sample_count);
  double voltage = ((value * 3300) / 4096);
  return voltage;
}