#ifndef EXBUS_H
#define EXBUS_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "escHW3.h"
#include "escHW4.h"
#include "escPWM.h"
#include "escCastle.h"
#include "escKontronik.h"
#include "voltage.h"
#include "ntc.h"
#include "pressure.h"
#include "bmp280.h"
#include "bn220.h"
#include "config.h"
#include "xbus.h"

#define EXBUSSERIAL_TIMEOUT 2

class Exbus
{
private:
    Stream &serial_;
#ifdef SOFTWARE_SERIAL
    SoftwareSerial softSerial = SoftwareSerial(PIN_SOFTSERIAL_RX, PIN_SOFTSERIAL_TX);
#endif
    uint16_t getCrc(uint8_t *buffer, uint8_t lenght);
    uint16_t byteCrc(uint16_t crc, uint8_t new_byte);

public:
    Exbus(Stream &serial);
    void begin();
    void checkSerial();
    void send();
};

#endif