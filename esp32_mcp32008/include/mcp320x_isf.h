#ifndef __MCP320X_H__
#define __MCP320X_H__

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "spi_driver.h"
#include <stdint.h>

#define MCP320X_RESOLUTION                                                     \
  4096 /** @brief ADC resolution = 12 bits = 2^12 = 4096 steps */
#define MCP320X_CLOCK_MIN_HZ                                                   \
  10000 /** @brief Minimum recommended clock speed for a reliable reading =    \
           10Khz. */
#define MCP320X_CLOCK_MAX_HZ                                                   \
  2000000 /** @brief Maximum clock speed supported = 2Mhz at 5V. */
#define MCP320X_REF_VOLTAGE_MIN                                                \
  250 /** @brief Minimum reference voltage, in mV = 250mV. */
#define MCP320X_REF_VOLTAGE_MAX                                                \
  3300 /** @brief Maximum reference voltage, in mV = 7000mV. The max safe      \
          voltage is 5000mV. */

/**
 * @typedef mcp320x_channel_t
 * @brief MPC320X channel.
 */

typedef enum {
  MCP320X_CHANNEL_0 = 0,
  MCP320X_CHANNEL_1 = 1,
  MCP320X_CHANNEL_2 = 2,
  MCP320X_CHANNEL_3 = 3,
  MCP320X_CHANNEL_4 = 4,
  MCP320X_CHANNEL_5 = 5,
  MCP320X_CHANNEL_6 = 6,
  MCP320X_CHANNEL_7 = 7
} mcp320x_channel_t;

/**
 * @typedef mcp320x_read_mode_t
 * @brief MCP320X read mode.
 */
typedef enum {
  MCP320X_READ_MODE_DIFFERENTIAL = 0,
  MCP320X_READ_MODE_SINGLE = 1
} mcp320x_read_mode_t;

uint16_t mcp_read(SPIDriver *spi, mcp320x_read_mode_t read_mode,
                  mcp320x_channel_t channel, uint16_t sample_count);

double mcp_read_voltage(SPIDriver *spi, mcp320x_read_mode_t read_mode,
                        mcp320x_channel_t channel, uint16_t sample_count);

#endif // __MCP320X_H__
