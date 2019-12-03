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
    for (int i = 0; i < samples; i++)
    {
        sensorData += analogRead(sensorPin);
    }
    sensorData /= samples;

    if (sensorData > sensorMax)
    {
        //TEST sensormax
        sensorMax = sensorData;
    }

    sensorData = (float)sensorData / sensorMax * 360;

    if (sensorData > 360.0)
    {
        sensorData = 360.0;
    }

    return sensorData;
}

Anemometer::Anemometer()
{
    armed = true;
}

void Anemometer::Setup(int _pin, long _breukteller)
{
    sensorPin = _pin;
    lastTimestamp = micros();
    breukteller = _breukteller;

    pinMode(sensorPin, INPUT);
}

float Anemometer::getSnelheid()
{
    //TODO rpm to m/s
    unsigned long tempInterval = micros() - lastTimestamp;

    if (tempInterval > interval)
    {
        if (tempInterval - interval > 3000000)
        {
            return 0;
        }
        else
        {
            return (float)breukteller / tempInterval;
        }
    }
    else
    {
        return (float)breukteller / interval;
    }
}

void Anemometer::Handle()
{
    if (armed && digitalRead(sensorPin))
    {
        armed = false;
        interval = micros() - lastTimestamp;
        lastTimestamp = micros();
    }
    else if (!armed && !digitalRead(sensorPin))
    {
        armed = true; //TODO debounce this shit
    }
}