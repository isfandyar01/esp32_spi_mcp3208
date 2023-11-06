#ifndef __RMS_CAL_H__
#define __RMS_CAL_H__
#include "mcp320x_isf.h"
#include "spi_driver.h"

#define ADC_BITS 12
#define ADC_COUNTS (1 << ADC_BITS)
#define moving_rms_max_buff 2048
static double ICAL = 90.9;

static double offsetI = (ADC_COUNTS >> 1);
static double filteredI;
static double sqV, sumV, sqI, sumI, instP, sumP;
static double realPower, apparentPower, powerFactor, Vrms;

typedef struct {
  uint16_t windown_lenght;
  float one_divided_by_window_lenght;
  uint16_t circular_buffer_count;
  float in_sq_L[moving_rms_max_buff];
  float out_sq;
} IFX_movingRms;

void init_IFX_movingRms(IFX_movingRms *mrms, uint16_t window_Len);

float IFX_MovingRms_Update(IFX_movingRms *mrms, float in);

double calcIrms_with_mcp3208(SPIDriver *spi, mcp320x_read_mode_t read_mode,
                             mcp320x_channel_t channel, uint16_t sample_count);

#endif // __RMS_CAL_H__
