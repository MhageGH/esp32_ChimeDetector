#include "arduinoFFT.h"
#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02

arduinoFFT FFT = arduinoFFT();
const uint16_t numSample = 1024; // This value MUST ALWAYS be a power of 2
double vReal[numSample];
double vImag[numSample];
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile boolean sampling = false;

void IRAM_ATTR onTimer(){
  portENTER_CRITICAL_ISR(&timerMux);
  sampling = true;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
  hw_timer_t* timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 23, true);
  timerAlarmEnable(timer);
  Serial.begin(115200);
}

void loop() {
  for (int i = 0; i < numSample; ++i) {
    while(!sampling);
    portENTER_CRITICAL(&timerMux);
    sampling = false;
    portEXIT_CRITICAL(&timerMux);
    vReal[i] = (float)((byte)((analogRead(34) >> 4) & 0xFF)) - 128.0;
    vImag[i] = 0.0;
  }
  FFT.Windowing(vReal, numSample, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, numSample, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, numSample);
  int i_max = 15;
  for (int i = 15; i <= 17; ++i) if (vReal[i] > vReal[i_max]) i_max = i;
  static int detectCount = 0;
  if (vReal[i_max] > 1500) ++detectCount;
  else detectCount = 0;
  if (detectCount == 3) Serial.println("Chime detect");
}

