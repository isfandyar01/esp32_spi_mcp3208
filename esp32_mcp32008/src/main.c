#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "math.h"
#include "mcp320x.h"

double offsetI = 2048;
double filteredI;
double sqV, sumV, sqI, sumI, instP, sumP;
double realPower, apparentPower, powerFactor, Vrms, Irms;

double calcIrms_with_mcp3208(int numberOfSamples, mcp320x_t *handle) {
  uint16_t sampleI = 0;
  double sumI = 0;

  mcp320x_acquire(handle, portMAX_DELAY);

  for (int n = 0; n < numberOfSamples; n++) {
    mcp320x_read(handle, MCP320X_CHANNEL_0, MCP320X_READ_MODE_SINGLE, 3000,
                 &sampleI);

    offsetI = (offsetI + (sampleI - offsetI) / 1024);
    filteredI = sampleI - offsetI;

    // Root-mean-square method current
    // 1) square current values
    sqI = filteredI * filteredI;
    // 2) sum
    sumI += sqI;
    vTaskDelay(1);
  }

  // Unoccupy the SPI bus.
  mcp320x_release(handle);

  double I_RATIO = 90.9 * ((3300 / 1000.0) / (4096));
  Irms = I_RATIO * sqrt(sumI / numberOfSamples);

  return Irms;
}

void app_main(void) {
  spi_bus_config_t bus_cfg = {.mosi_io_num = GPIO_NUM_23,
                              .miso_io_num = GPIO_NUM_19,
                              .sclk_io_num = GPIO_NUM_18,
                              .quadwp_io_num = -1,
                              .quadhd_io_num = -1,
                              .data4_io_num = -1,
                              .data5_io_num = -1,
                              .data6_io_num = -1,
                              .data7_io_num = -1,
                              .max_transfer_sz = 3, // 24 bits.
                              .flags = SPICOMMON_BUSFLAG_MASTER,
                              .intr_flags = ESP_INTR_FLAG_LEVEL3};

  mcp320x_config_t mcp320x_cfg = {.host = SPI3_HOST,
                                  .device_model = MCP3204_MODEL,
                                  .clock_speed_hz = 1 * 1000 * 1000, // 1 Mhz.
                                  .reference_voltage = 3300,         // 5V
                                  .cs_io_num = GPIO_NUM_22};

  // Bus initialization is up to the developer.
  spi_bus_initialize(mcp320x_cfg.host, &bus_cfg, 0);

  // Add the device to the SPI bus.
  mcp320x_t *mcp320x_handle = mcp320x_install(&mcp320x_cfg);

  ESP_LOGI("mcp320x", "Initializaing readings");

  while (1) {
    // Occupy the SPI bus for multiple transactions.
    double irms = calcIrms_with_mcp3208(2000, mcp320x_handle);

    ESP_LOGI("mcp320x", "Current: %f mA", irms);

    vTaskDelay(pdMS_TO_TICKS(1000));
  }

  // Free resources.
  mcp320x_delete(mcp320x_handle);
}