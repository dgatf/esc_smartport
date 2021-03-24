#include "bst.h"

#if CONFIG_VOLTAGE1 || CONFIG_CURRENT
Bst_Battery Bst::bstBattery;
#endif
#if CONFIG_GPS
Bst_Gps_Position Bst::bstGpsPosition;
#endif

Bst::Bst()
{
}

void Bst::sendData()
{
    static uint16_t ts = 0;
    static uint8_t cont = 0;
    if ((uint16_t)millis() - ts > 1000)
    {
        uint8_t buffer[20] = {0};
        uint8_t len = 0;
        switch (cont)
        {
        case 0:
#if CONFIG_VOLTAGE1 || CONFIG_CURRENT
            len = sizeof(bstBattery);
            memcpy(buffer, (uint8_t *)&bstBattery, len);
#endif
            break;
        case 1:
#if CONFIG_GPS
            len = sizeof(bstGpsPosition);
            memcpy(buffer, (uint8_t *)&bstGpsPosition, len);
#endif
            break;
        case 2:
#if CONFIG_AIRSPEED
            len = sizeof(bstAirspeed);
            memcpy(buffer, (uint8_t *)&bstAirspeed, len);
#endif
            break;
        default:
            break;
        }
        if (len > 0)
        {
            buffer[len] = getCrc(buffer, len);
            Wire.beginTransmission(0); // broadcast
            Wire.write(buffer, len + 1);
            Wire.endTransmission();
            ts = millis();
#ifdef DEBUG
            for (int i = 0; i <= len; i++)
            {
                DEBUG_SERIAL.print(buffer[i], HEX);
                DEBUG_SERIAL.print(" ");
            }
            DEBUG_SERIAL.println();
#endif
        }
        cont++;
        if (cont == 3)
            cont = 0;
    }
}

void Bst::begin()
{
    pinMode(LED_BUILTIN, OUTPUT);
#if CONFIG_GPS
    GPS_SERIAL.begin(GPS_BAUD_RATE);
    GPS_SERIAL.setTimeout(BN220_TIMEOUT);
#endif
    Wire.begin();
    Wire.setWireTimeout(1000);
}

void Bst::update()
{
#if CONFIG_VOLTAGE1
    volt1.update();
    bstBattery.voltage = __builtin_bswap16((uint16_t)(*volt1.valueP() * 10));
#endif
#if CONFIG_CURRENT
    curr.update();
    bstBattery.current = __builtin_bswap16((uint16_t)(*curr.valueP() * 10));
#endif
#if CONFIG_GPS
    gps.update();
    bstGpsPosition.latitude = __builtin_bswap32((uint32_t)(*gps.latP() / 60 * 1E7));
    bstGpsPosition.longitude = __builtin_bswap32((uint32_t)(*gps.lonP() / 60 * 1E7));
    bstGpsPosition.speed = __builtin_bswap16((uint16_t)(*gps.spdP() * 10));
    bstGpsPosition.course = __builtin_bswap16((uint16_t)(*gps.cogP() * 100));
    bstGpsPosition.sat = *gps.satP();
    bstGpsPosition.altitude = __builtin_bswap16((uint16_t)(*gps.altP() + 1000));
#endif
#if CONFIG_AIRSPEED
    airspeed.update();
    bstAirspeed.airspeed = __builtin_bswap16((uint16_t)(*airspeed.valueP()));
#endif
    sendData();
}

uint8_t Bst::getCrc(uint8_t *buffer, uint8_t len)
{
    uint8_t crc = 0;
    for (uint8_t i = 0; i < len; i++)
        getByteCrc(buffer[i], crc);
    getByteCrc(0, crc);
    return crc;
}

void Bst::getByteCrc(uint8_t data, uint8_t &crc)
{
    bool flag;
    uint8_t polynomial = 0xD5;
    for (uint8_t i = 0; i < 8; i++)
    {
        flag = false;
        if (crc & 0x80)
            flag = true;
        crc <<= 1;
        if (data & 0x80)
            crc++;
        data <<= 1;
        if (flag == true)
            crc ^= polynomial;
    }
}