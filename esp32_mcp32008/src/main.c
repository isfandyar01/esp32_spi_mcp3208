

#include "esp_log.h"
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mcp320x_isf.h"
#include <math.h>
#include <stdio.h>

#define NUMBER_OF_SAMPLES 1048

static const char *TAG = "current_sensor";
static const char *VOLTTAG = " Voltage";

double offsetI;
double filteredI;
double sqV, sumV, sqI, sumI, instP, sumP;
double realPower, apparentPower, powerFactor, Vrms;

static uint16_t spi_read() {
  esp_err_t ret;

  spi_bus_config_t buscfg = {
      .mosi_io_num = GPIO_NUM_23,
      .miso_io_num = GPIO_NUM_19,
      .sclk_io_num = GPIO_NUM_18,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
  };

  ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
  ESP_ERROR_CHECK(ret);

  spi_device_handle_t spi2;

  spi_device_interface_config_t dev_config;
  dev_config.address_bits = 0;
  dev_config.command_bits = 0;
  dev_config.dummy_bits = 0;
  dev_config.mode = 0;
  dev_config.duty_cycle_pos = 0;
  dev_config.cs_ena_posttrans = 0;
  dev_config.cs_ena_pretrans = 0;
  dev_config.clock_speed_hz = 10 * 1000 * 1000;
  dev_config.clock_source = SPI_CLK_SRC_DEFAULT;
  dev_config.spics_io_num = GPIO_NUM_5;
  dev_config.flags = SPI_DEVICE_NO_DUMMY;
  dev_config.queue_size = 1;
  dev_config.pre_cb = NULL;
  dev_config.post_cb = NULL;

  ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &dev_config, &spi2));

  uint16_t value = 0;
  uint32_t sum = 0;
  char data[3];
  char rxdata[3];
  // spi_transaction_t transaction = {
  //     .flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA, .length = 24};

  spi_transaction_t trans_desc;
  trans_desc.addr = 0;
  trans_desc.cmd = 0;
  trans_desc.flags = SPI_TRANS_USE_RXDATA;
  trans_desc.length = 3 * 8;
  trans_desc.rxlength = 3 * 8;
  // trans_desc.rx_buffer = data;

  trans_desc.tx_data[0] = (uint8_t)((1 << 2) | (MCP320X_READ_MODE_SINGLE << 1) |
                                    ((MCP320X_CHANNEL_0 & 4) >> 2));
  trans_desc.tx_data[1] = (uint8_t)(MCP320X_CHANNEL_0 << 6);
  trans_desc.tx_data[2] = 0;

  for (uint16_t i = 0; i < 3000; i++) {
    ESP_ERROR_CHECK(spi_device_polling_transmit(spi2, &trans_desc));
    const uint16_t first_part = trans_desc.rx_data[1];
    const uint16_t second_part = trans_desc.rx_data[2];

    sum += ((first_part & 15) << 8) | second_part;
  }
  ESP_ERROR_CHECK(spi_bus_remove_device(spi2));
  ESP_ERROR_CHECK(spi_bus_free(SPI2_HOST));
  value = (uint16_t)(sum / 2000);
  return value;
};

double calcIrms_with_mcp3208(int numberOfSamples) {
  uint16_t sampleI = 0;
  double sumI = 0;

  for (int n = 0; n < numberOfSamples; n++) {
    sampleI = spi_read();

    offsetI = (offsetI + (sampleI - offsetI) / 1024);
    filteredI = sampleI - offsetI;

    // Root-mean-square method current
    // 1) square current values
    sqI = filteredI * filteredI;
    // 2) sum
    sumI += sqI;
  }

  double I_RATIO = 90.9 * ((3300 / 1000.0) / (4096));
  double Irms = I_RATIO * sqrt(sumI / numberOfSamples);

  sumI = 0;

  return Irms;
}

void app_main() {

  while (1) {

    double Irms_main = calcIrms_with_mcp3208(10);
    ESP_LOGI("mcp320x", "Current: %f mA", Irms_main);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}