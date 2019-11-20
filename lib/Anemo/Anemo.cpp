//TODO automatic sensormax updating

#include "Anemo.h"

Windvaan::Windvaan()
{
}

void Windvaan::Setup(int _pin, int _sensorMax, int _sensorMin, int _samples)
{
    sensorPin = _pin;
    sensorMax = _sensorMax;
    sensorMin = _sensorMin;
    samples = _samples;

    if (samples < 1)
        samples = 1;
    if (samples > 1000)
        samples = 1000;

    pinMode(sensorPin, INPUT);
}

float Windvaan::getRichting()
{
    for (size_t i = 0; i < samples; i++)
    {
        sensorData += analogRead(sensorPin);
    }
    sensorData /= samples;
    //TODO automatic

    return (float)sensorData / sensorMax * 360.0;
}