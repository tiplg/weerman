//TODO automatic sensormax updating

#include "Anemo.h"

void interrupt();

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
    for (int i = 0; i < samples; i++)
    {
        sensorData += analogRead(sensorPin);
    }
    sensorData /= samples;

    return (float)sensorData / sensorMax * 360.0;
}

Anemometer *a;

Anemometer::Anemometer()
{
}

void Anemometer::Setup(int _pin, int _breukteller)
{
    a = this;

    sensorPin = _pin;
    lastTimestamp = millis();
    breukteller = _breukteller;

    pinMode(sensorPin, INPUT);

    attachInterrupt(sensorPin, interrupt, FALLING);
}

float Anemometer::getSnelheid()
{
    //TODO improve for low intervals
    sensorData = (float)breukteller / interval;
    return sensorData;
}

void Anemometer::HandleInterrupt()
{
    interval = millis() - lastTimestamp;
    lastTimestamp = millis();
}

void ICACHE_RAM_ATTR interrupt() //TODO debounce this shit
{
    a->HandleInterrupt();
}
