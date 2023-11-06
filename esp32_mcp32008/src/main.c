
#include "Rms_cal.h"
#include "mcp320x_isf.h"
#include "soc/timer_group_reg.h"
#include "soc/timer_group_struct.h"
#include "spi_driver.h"
#include <stdio.h>

void app_main(void) {
  double volts = 0;
  double Irms = 0;
  SPIDriver spi;
  esp_err_t ret;
  IFX_movingRms RMS;

  // init_IFX_movingRms(&RMS, 2048);

  spi.host = SPI2_HOST;

  ret = spi_driver_init(&spi, GPIO_NUM_5);

  if (ret != ESP_OK) {
    printf("SPI initialization error: %s\n", esp_err_to_name(ret));
  }
  while (1) {

    // uint16_t value = mcp_read_without_averaging_samples(
    //     &spi, MCP320X_READ_MODE_SINGLE, MCP320X_CHANNEL_0);
    Irms = calcIrms_with_mcp3208(&spi, MCP320X_READ_MODE_SINGLE,
                                 MCP320X_CHANNEL_0, 10000);

    ESP_LOGI("mcp320x", "Current: %f mA", Irms);
  }
}