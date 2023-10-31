#ifndef __RMS_CAL_H__
#define __RMS_CAL_H__

double ICAL = 90.9;
static const char *TAG = "current_sensor";
static const char *VOLTTAG = " Voltage";

double offsetI = 2048;
double filteredI;
double sqV, sumV, sqI, sumI, instP, sumP;
double realPower, apparentPower, powerFactor, Vrms;

#endif // __RMS_CAL_H__

double calcIrms_with_mcp3208(int numberOfSamples, mcp320x_t *handle);