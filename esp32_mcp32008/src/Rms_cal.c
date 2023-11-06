#include "Rms_cal.h"
#include "math.h"
#include "mcp320x_isf.h"

void init_IFX_movingRms(IFX_movingRms *mrms, uint16_t window_Len) {

  mrms->windown_lenght = window_Len;
  mrms->one_divided_by_window_lenght = 1.0f / ((float)window_Len);
  mrms->circular_buffer_count = 0;

  for (u_int16_t i = 0; i < window_Len; i++) {
    mrms->in_sq_L[i] = 0.0f;
  }
  mrms->out_sq = 0.0f;
}

float IFX_MovingRms_Update(IFX_movingRms *mrms, float in) {
  float in_sq = in * in;
  float n_minus_L = mrms->in_sq_L[mrms->circular_buffer_count];
  mrms->in_sq_L[mrms->circular_buffer_count] = in_sq;

  if (mrms->circular_buffer_count == (mrms->windown_lenght - 1)) {
    mrms->circular_buffer_count = 0;
  } else {
    mrms->circular_buffer_count++;
  }
  mrms->out_sq =
      mrms->out_sq + mrms->one_divided_by_window_lenght *
                         (in_sq - n_minus_L); // Subtract the saved n-L value
  return mrms->out_sq;
}

double calcIrms_with_mcp3208(SPIDriver *spi, mcp320x_read_mode_t read_mode,
                             mcp320x_channel_t channel, uint16_t sample_count) {
  uint16_t sampleI = 0;
  double sumI = 0;

  for (int n = 0; n <= sample_count; n++) {
    sampleI = mcp_read_without_averaging_samples(spi, read_mode, channel);
    //  ESP_LOGI(TAG, "Single_ended ch0 value=%d", sampleI);
    offsetI = (offsetI + (sampleI - offsetI) / 1024);
    filteredI = sampleI - offsetI;

    sqI = filteredI * filteredI;
    sumI += sqI;
  }

  double I_RATIO = ICAL * ((3300 / 1000.0) / (4096));
  double Irms = I_RATIO * sqrt(sumI / sample_count);

  sumI = 0;

  return Irms;
}
