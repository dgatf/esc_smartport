#ifndef BST_H
#define BST_H

#define BST_I2C_GPS (0xC2 >> 1)
#define BST_I2C_BATTERY (0xC0 >> 1)
#define BST_I2C_AIRSPEED (0xC4 >> 1)

#define BST_FRAME_ID_GPS_POSITION 0x02
#define BST_FRAME_ID_GPS_TIME 0x03
#define BST_FRAME_ID_AIRSPEED 0x07
#define BST_FRAME_ID_BATTERY 0x08

#include <Arduino.h>
#include <Wire.h>
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

struct Bst_Battery
{
    uint8_t frameId = BST_FRAME_ID_BATTERY;
    uint16_t voltage = 0; // Voltage ( mV * 100 )
    uint16_t current = 0; // Current ( mA * 100 )
    uint32_t fuel = 0;    // Fuel (24bits) ( drawn mAh )
    //int8_t remaining;
};

struct Bst_Gps_Position
{
    uint8_t frameId = BST_FRAME_ID_GPS_POSITION;
    int32_t latitude = 0;  // Latitude ( degree / 10`000`000 )
    int32_t longitude = 0; // Longitude (degree / 10`000`000 )
    uint16_t speed = 0;    // Groundspeed ( km/h / 10 )
    uint16_t course = 0;   // GPS heading ( degree / 100 )
    uint16_t altitude = 0; // Altitude ( meter ­1000m offset )
    uint8_t sat = 0;       // Satellites in use ( counter )
    //uint8_t spare = 0;
};

struct Bst_Airspeed
{
    uint8_t frameId = BST_FRAME_ID_AIRSPEED;
    uint16_t speed = 0;
};

class Bst
{
private:
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega32U4__)
    uint8_t addressMask = 0;
#endif
    static void i2c_request_handler();

protected:
#ifdef SOFTWARE_SERIAL
    SoftwareSerial softSerial = SoftwareSerial(PIN_SOFTSERIAL_RX, PIN_SOFTSERIAL_TX);
#endif
#if CONFIG_VOLTAGE1 || CONFIG_CURRENT
    static Bst_Battery bstBattery;
#endif
#if CONFIG_VOLTAGE1
    Voltage volt1 = Voltage(PIN_VOLTAGE1, ALPHA(CONFIG_AVERAGING_ELEMENTS_VOLT));
#endif
#if CONFIG_CURRENT
    Voltage curr = Voltage(PIN_CURRENT, ALPHA(CONFIG_AVERAGING_ELEMENTS_CURR));
#endif
#if CONFIG_GPS
    static Bst_Gps_Position bstGpsPosition;
    Bn220 gps = Bn220(GPS_SERIAL);
#endif
#if CONFIG_AIRSPEED
    static Bst_Airspeed bstAirspeed;
    Pressure airspeed = Pressure(PIN_PRESSURE, 1);
#endif

private:
    static void getByteCrc(uint8_t data, uint8_t &crc);
    static uint8_t getCrc(uint8_t *buffer, uint8_t len);

public:
    Bst();
    void begin();
    void update();
};

#endif