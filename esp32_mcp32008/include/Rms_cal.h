#ifndef __RMS_CAL_H__
#define __RMS_CAL_H__
#include "mcp320x_isf.h"

#define ADC_BITS 12
#define ADC_COUNTS (1 << ADC_BITS)
static double ICAL = 90.9;

static double offsetI = (ADC_COUNTS >> 1);
static double filteredI;
static double sqV, sumV, sqI, sumI, instP, sumP;
static double realPower, apparentPower, powerFactor, Vrms;

#endif // __RMS_CAL_H__

double calcIrms_with_mcp3208(int numberOfSamples);