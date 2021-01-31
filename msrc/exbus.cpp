#include "exbus.h"

Exbus::Exbus(Stream &serial) : serial_(serial) {}

void Exbus::begin()
{
    pinMode(LED_BUILTIN, OUTPUT);
    
}


void Exbus::send()
{
  
}

void Exbus::checkSerial()
{

}