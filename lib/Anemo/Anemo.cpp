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
    for (int i = 0; i < samples; i++)
    {
        sensorData += analogRead(sensorPin);
    }
    sensorData /= samples;

    //TODO limit to 360

    return (float)sensorData / sensorMax * 360.0;
}

Anemometer::Anemometer()
{
    armed = true;
}

void Anemometer::Setup(int _pin, int _breukteller)
{
    sensorPin = _pin;
    lastTimestamp = millis();
    breukteller = _breukteller;

    pinMode(sensorPin, INPUT);
}

float Anemometer::getSnelheid()
{
    //TODO improve for low intervals
    sensorData = (float)breukteller / interval;
    return sensorData;
}

void Anemometer::Handle()
{
    if (armed && digitalRead(sensorPin))
    {
        //TODO test with micros()
        armed = false;
        interval = millis() - lastTimestamp;
        lastTimestamp = millis();
    }
    else if (!armed && !digitalRead(sensorPin))
    {
        armed = true; //TODO debounce this shit
    }
}