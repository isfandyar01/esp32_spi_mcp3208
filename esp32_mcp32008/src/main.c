
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mcp3002.h"
#include "mcp320x_isf.h"
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// #define TAG "MCP3208"
#define NUMBER_OF_SAMPLES 1048
double ICAL = 90.9;
static const char *TAG = "current_sensor";
static const char *VOLTTAG = " Voltage";

double offsetI = 2048;
double filteredI;
double sqV, sumV, sqI, sumI, instP, sumP;
double realPower, apparentPower, powerFactor, Vrms;
int model = MCP3208;
// Declare the SPI bus and device handles as global variables
static spi_device_handle_t spi2;
MCP_t dev;

double calcIrms_with_mcp3208(int numberOfSamples) {
  int sampleI = 0;
  double sumI = 0;

  for (int n = 0; n <= numberOfSamples; n++) {
    sampleI = mcpReadData(&dev, 7);
    // ESP_LOGI(TAG, "Single_ended ch0 value=%d", sampleI);
    offsetI = (offsetI + (sampleI - offsetI) / 1024);
    filteredI = sampleI - offsetI;

    sqI = filteredI * filteredI;
    sumI += sqI;
  }

  double I_RATIO = ICAL * ((3300 / 1000.0) / (4096));
  double Irms = I_RATIO * sqrt(sumI / numberOfSamples);

  sumI = 0;

  return Irms;
}

void app_main() {
  int16_t value;
  // Initialize the SPI bus
  mcpInit(&dev, model, GPIO_NUM_19, GPIO_NUM_23, GPIO_NUM_18, GPIO_NUM_5,
          MCP_SINGLE);

  while (1) {
    // double Irms_main = calcIrms_with_mcp3208(1048);
    value = mcpReadData(&dev, 1);
    ESP_LOGI("mcp320x", "Current: %d mA", value);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}