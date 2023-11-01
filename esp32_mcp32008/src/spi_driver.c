#include "spi_driver.h"
#include "stdio.h"
#include <string.h>
esp_err_t spi_driver_init(SPIDriver *spi, gpio_num_t cs_pin) {
  esp_err_t ret;

  spi_bus_config_t bus_config = {
      .miso_io_num = GPIO_NUM_19,
      .mosi_io_num = GPIO_NUM_23,
      .sclk_io_num = GPIO_NUM_18,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
  };

  ret = spi_bus_initialize(spi->host, &bus_config, 0);
  if (ret != ESP_OK) {
    return ret;
  }

  spi_device_interface_config_t dev_config = {
      .command_bits = 0,
      .address_bits = 0,
      .dummy_bits = 0,
      .mode = 0,
      .duty_cycle_pos = 128,
      .cs_ena_pretrans = 0,
      .cs_ena_posttrans = 0,
      .clock_speed_hz = 1000000, // 1 MHz
      .input_delay_ns = 0,
      .spics_io_num = cs_pin,
      .flags = 0,
      .queue_size = 7,
      .pre_cb = NULL,
      .post_cb = NULL,
  };

  ret = spi_bus_add_device(spi->host, &dev_config, &spi->spi);
  if (ret != ESP_OK) {
    return ret;
  }

  spi->cs_pin = cs_pin;

  return ESP_OK;
}

// Transmit data over SPI
esp_err_t spi_driver_transfer(SPIDriver *spi, const uint8_t *tx_data,
                              size_t tx_length, uint8_t *rx_data,
                              size_t rx_length) {

  spi_transaction_t trans;
  memset(&trans, 0, sizeof(trans));

  trans.length = tx_length * 8;
  trans.rxlength = rx_length * 8;
  trans.tx_buffer = tx_data;
  trans.rx_buffer = rx_data;

  return spi_device_polling_transmit(spi->spi, &trans);
}

// Deinitialize the SPI driver
esp_err_t spi_driver_deinit(SPIDriver *spi) {
  spi_bus_remove_device(spi->spi);
  spi_bus_free(spi->host);
  return ESP_OK;
}
