#include "Rms_cal.h"
#include "math.h"
#include "mcp320x.h"

double calcIrms_with_mcp3208(int numberOfSamples, mcp320x_t *handle) {
  uint16_t sampleI = 0;
  double sumI = 0;

  for (int n = 0; n <= numberOfSamples; n++) {
    mcp320x_read(handle, MCP320X_CHANNEL_0, MCP320X_READ_MODE_SINGLE, 2000,
                 &sampleI);
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
