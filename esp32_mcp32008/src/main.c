
#include "mcp320x_isf.h"
#include "soc/timer_group_reg.h"
#include "soc/timer_group_struct.h"
#include "spi_driver.h"
#include <stdio.h>

void app_main(void) {
  uint16_t value = 0;
  SPIDriver spi;
  esp_err_t ret;

  spi.host = SPI2_HOST;

  ret = spi_driver_init(&spi, GPIO_NUM_5);

  if (ret != ESP_OK) {
    printf("SPI initialization error: %s\n", esp_err_to_name(ret));
  }
  while (1) {
    value = mcp_read_voltage(&spi, MCP320X_READ_MODE_SINGLE, MCP320X_CHANNEL_0,
                             1000);
    ESP_LOGI("mcp320x", "Current: %d mA", value);
  }
}