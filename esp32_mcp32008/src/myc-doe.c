

// #include "Ads1115.h"
// #include "driver/adc.h"
// #include "driver/gpio.h"
// #include "driver/spi_master.h"
// #include "esp_log.h"
// #include "mcp320x.h"

// #include "driver/i2c.h"

// #include "esp_adc_cal.h"
// #include "esp_log.h"
// #include "esp_types.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "mcp320x.h"
// #include <math.h>
// #include <stdio.h>

// void app_main(void) {
//   spi_bus_config_t bus_cfg = {.mosi_io_num = GPIO_NUM_23,
//                               .miso_io_num = GPIO_NUM_19,
//                               .sclk_io_num = GPIO_NUM_18,
//                               .quadwp_io_num = -1,
//                               .quadhd_io_num = -1,
//                               .data4_io_num = -1,
//                               .data5_io_num = -1,
//                               .data6_io_num = -1,
//                               .data7_io_num = -1,
//                               .max_transfer_sz = 3, // 24 bits.
//                               .flags = SPICOMMON_BUSFLAG_MASTER,
//                               // .isr_cpu_id = INTR_CPU_ID_AUTO,
//                               .intr_flags = ESP_INTR_FLAG_LEVEL3};

//   mcp320x_config_t mcp320x_cfg = {.host = SPI3_HOST,
//                                   .device_model = MCP3204_MODEL,
//                                   .clock_speed_hz = 1 * 1000 * 1000, // 1
//                                   Mhz. .reference_voltage = 3300,         //
//                                   5V .cs_io_num = GPIO_NUM_5};

//   // Bus initialization is up to the developer.
//   spi_bus_initialize(mcp320x_cfg.host, &bus_cfg, 0);

//   // Add the device to the SPI bus.
//   mcp320x_t *mcp320x_handle = mcp320x_install(&mcp320x_cfg);

//   // Occupy the SPI bus for multiple transactions.
//   mcp320x_acquire(mcp320x_handle, portMAX_DELAY);

//   uint16_t voltage = 0;
//   while (1) {
//     // Read voltage, sampling 1000 times.
//     mcp320x_read(mcp320x_handle, MCP320X_CHANNEL_0, MCP320X_READ_MODE_SINGLE,
//                  1000, &voltage);

//     // Unoccupy the SPI bus.
//     // mcp320x_release(mcp320x_handle);

//     // Free resources.
//     // mcp320x_delete(mcp320x_handle);

//     ESP_LOGI("mcp320x", "Voltage: %d mV", voltage);
//     vTaskDelay(1000 / portTICK_PERIOD_MS);
//   }
// }
/**
 * @brief  isf implementation of spi and mcp read
 *
 *
 */
// #include "Ads1115.h"
// #include "MCP3208.h"
// #include "driver/adc.h"
// #include "esp_adc_cal.h"
// #include "esp_log.h"
// #include "esp_types.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "mcp320x.h"
// #include <math.h>
// #include <stdio.h>

// #define ADS1115_ADDRESS 0x48
// #define SDA_GPIO 21
// #define SCL_GPIO 22
// #define multiplication_factor 10
// #define NUMBER_OF_SAMPLES 1048

// // static const char *TAG = "main";
// static ads1115_t ads;
// static const char *TAG = "current_sensor";
// static const char *VOLTTAG = " Voltage";

// double offsetI;
// double filteredI;
// double sqV, sumV, sqI, sumI, instP, sumP;
// double realPower, apparentPower, powerFactor, Vrms, Irms;

// spi_device_handle_t spi2;

// static void spi_init() {
//   esp_err_t ret;

//   spi_bus_config_t buscfg = {
//       .mosi_io_num = GPIO_NUM_23,
//       .miso_io_num = GPIO_NUM_19,
//       .sclk_io_num = GPIO_NUM_18,
//       .quadwp_io_num = -1,
//       .quadhd_io_num = -1,
//       .max_transfer_sz = 32,
//   };

//   ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
//   ESP_ERROR_CHECK(ret);

//   spi_device_interface_config_t devcfg = {
//       .clock_speed_hz = 1000000, // 1 MHz
//       .mode = 0,                 // SPI mode 0
//       .spics_io_num = GPIO_NUM_5,
//       .queue_size = 32,
//       .flags = SPICOMMON_BUSFLAG_MASTER,
//       .pre_cb = NULL,
//       .post_cb = NULL,
//   };

//   ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &spi2));
// };

// // uint16_t mcp_read_main(mcp320x_read_mode_t read_mode, mcp320x_channel_t
// // channel,
// //                        uint16_t sample_count) {
// //   uint16_t value = 0;
// //   uint32_t sum = 0;

// //   spi_transaction_t transaction = {
// //       .flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA, .length = 24};

// //   transaction.tx_data[0] =
// //       (uint8_t)((1 << 2) | (read_mode << 1) | ((channel & 4) >> 2));
// //   transaction.tx_data[1] = (uint8_t)(channel << 6);
// //   transaction.tx_data[2] = 0;

// //   for (uint16_t i = 0; i < sample_count; i++) {
// //     ESP_ERROR_CHECK(spi_device_polling_transmit(spi2, &transaction));
// //     const uint16_t first_part = transaction.rx_data[1];
// //     const uint16_t second_part = transaction.rx_data[2];

// //     sum += ((first_part & 15) << 8) | second_part;
// //   }

// //   value = (uint16_t)(sum / sample_count);
// //   return value;
// // }

// double calcIrms_with_mcp3208(int numberOfSamples) {
//   uint16_t sampleI = 0;
//   double sumI = 0;

//   for (int n = 0; n < numberOfSamples; n++) {

//     sampleI = mcp_read(spi2, MCP320X_READ_MODE_SINGLE, MCP320X_CHANNEL_0,
//     1000);

//     offsetI = (offsetI + (sampleI - offsetI) / 1024);
//     filteredI = sampleI - offsetI;

//     // Root-mean-square method current
//     // 1) square current values
//     sqI = filteredI * filteredI;
//     // 2) sum
//     sumI += sqI;
//   }
//   // Unoccupy the SPI bus.

//   double I_RATIO = 90.9 * ((3300 / 1000.0) / (4096));
//   Irms = I_RATIO * sqrt(sumI / numberOfSamples);

//   // Reset accumulators
//   sumI = 0;
//   // mcp320x_release(handle);
//   //--------------------------------------------------------------------------------------

//   return Irms;
// }

// void app_main() {
//   spi_init();
//   // int volts = 0;

//   while (1) {
//     // uint16_t volts =
//     //     mcp_read(spi2, MCP320X_READ_MODE_SINGLE, MCP320X_CHANNEL_0, 1000);
//     // volts = ((volts * 3300) / 4096);
//     // ESP_LOGI("mcp320x", "Voltage: %d mV", volts);
//     // double Irms = calcIrms_with_mcp3208(NUMBER_OF_SAMPLES);
//     vTaskDelay(1000 / portTICK_PERIOD_MS);
//   }
// }

// #include "MCP3208.h"
// #include "driver/gpio.h"
// #include "driver/spi_master.h"
// #include "esp_log.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "math.h"

// double offsetI;
// double filteredI;
// double sqV, sumV, sqI, sumI, instP, sumP;
// double realPower, apparentPower, powerFactor, Vrms, Irms;

// double calcIrms_with_mcp3208(int numberOfSamples) {

//   spi_bus_config_t bus_cfg = {.mosi_io_num = GPIO_NUM_23,
//                               .miso_io_num = GPIO_NUM_19,
//                               .sclk_io_num = GPIO_NUM_18,
//                               .quadwp_io_num = -1,
//                               .quadhd_io_num = -1,
//                               .data4_io_num = -1,
//                               .data5_io_num = -1,
//                               .data6_io_num = -1,
//                               .data7_io_num = -1,
//                               .max_transfer_sz = 3, // 24 bits.
//                               .flags = SPICOMMON_BUSFLAG_MASTER,
//                               .isr_cpu_id = INTR_CPU_ID_AUTO,
//                               .intr_flags = ESP_INTR_FLAG_LEVEL3};

//   mcp320x_config_t mcp320x_cfg = {.host = SPI3_HOST,
//                                   .device_model = MCP3204_MODEL,
//                                   .clock_speed_hz = 1 * 1000 * 1000, // 1
//                                   Mhz. .reference_voltage = 5000,         //
//                                   5V .cs_io_num = GPIO_NUM_5};

//   // Bus initialization is up to the developer.
//   spi_bus_initialize(mcp320x_cfg.host, &bus_cfg, 0);

//   // Add the device to the SPI bus.
//   mcp320x_t *mcp320x_handle = mcp320x_install(&mcp320x_cfg);

//   // Occupy the SPI bus for multiple transactions.
//   mcp320x_acquire(mcp320x_handle, portMAX_DELAY);

//   uint16_t voltage = 0;
//   // mcp320x_acquire(mcp320x_handle, portMAX_DELAY);
//   uint16_t sampleI = 0;
//   double sumI = 0;
//   for (int n = 0; n < numberOfSamples; n++) {

//     mcp320x_read(mcp320x_handle, MCP320X_READ_MODE_SINGLE, MCP320X_CHANNEL_0,
//                  3000, &sampleI);

//     offsetI = (offsetI + (sampleI - offsetI) / 1024);
//     filteredI = sampleI - offsetI;

//     // Root-mean-square method current
//     // 1) square current values
//     sqI = filteredI * filteredI;
//     // 2) sum
//     sumI += sqI;
//     //
//     vTaskDelay(2);
//   }
//   mcp320x_release(mcp320x_handle);

//   // Free resources.
//   mcp320x_delete(mcp320x_handle);

//   double I_RATIO = 90.9 * ((3300 / 1000.0) / (4096));
//   Irms = I_RATIO * sqrt(sumI / numberOfSamples);

//   // Reset accumulators
//   sumI = 0;

//   //--------------------------------------------------------------------------------------

//   return Irms;
// }

// void app_main() {

//   uint16_t voltage = 0;
//   double irms = 0;
//   while (1) {
//     // Occupy the SPI bus for multiple transactions.
//     irms = calcIrms_with_mcp3208(1048);

//     ESP_LOGI("mcp320x", "Current: %f mA", irms);

//     vTaskDelay(1000 / portTICK_PERIOD_MS);
//   }
// }

// double calcIrms_with_mcp3208(int numberOfSamples) {
//   uint16_t sampleI = 0;
//   double sumI = 0;

//   for (int n = 0; n < numberOfSamples; n++) {

//     sampleI = mcp_read(spi2, MCP320X_READ_MODE_SINGLE, MCP320X_CHANNEL_0,
//     1000);

//     offsetI = (offsetI + (sampleI - offsetI) / 1024);
//     filteredI = sampleI - offsetI;

//     // Root-mean-square method current
//     // 1) square current values
//     sqI = filteredI * filteredI;
//     // 2) sum
//     sumI += sqI;
//   }
//   // Unoccupy the SPI bus.

//   double I_RATIO = 90.9 * ((3300 / 1000.0) / (4096));
//   Irms = I_RATIO * sqrt(sumI / numberOfSamples);

//   sumI = 0;

//   return Irms;
// }