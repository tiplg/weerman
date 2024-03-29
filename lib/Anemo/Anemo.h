#ifndef Anemo_h
#define Anemo_h

#include <Arduino.h>

class Windvaan
{
public:
    Windvaan();

    void Setup(int _pin, int _sensorMax, int _sensorMin, int _samples);
    float getRichting();

    int sensorPin;
    int sensorMax;
    int sensorMin;
    int sensorData;
    int samples;
};

class Anemometer
{
public:
    Anemometer();

    void Setup(int _pin, long _breukteller);
    float getSnelheid();
    void HandleInterrupt();
    void Handle();

    int sensorPin;

    bool armed;
    float sensorData;
    long breukteller;
    unsigned long lastTimestamp;
    unsigned long interval;
};

#endif