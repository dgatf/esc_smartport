#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>
#include "config.h"

class AbstractDevice
{
private:
protected:
    float calcAverage(float alpha, float value, float newValue);

public:
    AbstractDevice();
    virtual ~AbstractDevice();
    virtual void update() = 0;
};

#endif