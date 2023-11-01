#ifndef __SPI_DRIVER_H__
#define __SPI_DRIVER_H__



#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "driver/spi_common.h"


typedef struct {
    spi_device_handle_t spi;
    gpio_num_t cs_pin;
    spi_host_device_t host;
} SPIDriver;


esp_err_t spi_driver_init(SPIDriver* spi, gpio_num_t cs_pin);
esp_err_t spi_driver_deinit(SPIDriver* spi);
esp_err_t spi_driver_transfer(SPIDriver* spi, const uint8_t* tx_data, size_t tx_length, uint8_t* rx_data, size_t rx_length);

#endif // __SPI_DRIVER_H__
