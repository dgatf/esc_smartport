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

void Bst::i2c_request_handler()
{
#ifdef SIM_RX
    uint8_t list[] = {BST_I2C_BATTERY, BST_I2C_GPS};
    static uint8_t cont = 0;
    uint8_t address = list[cont];
    cont++;
    if (cont > 1)
        cont = 0;
#else
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega32U4__)
    uint8_t address = TWDR >> 1;
#endif
#if defined(__MKL26Z64__) || defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
    uint8_t address = I2C0_D >> 1;
#endif
#endif
#ifdef DEBUG
    DEBUG_SERIAL.print("I2CA: ");
    DEBUG_SERIAL.println(address, HEX);
#endif
    uint8_t buffer[20] = {0};
    uint8_t len;
    switch (address)
    {
#if CONFIG_VOLTAGE1 || CONFIG_CURRENT
    case BST_I2C_BATTERY:
        len = sizeof(bstBattery);
        memcpy(buffer, (uint8_t *)&bstBattery, len);
        break;
#endif
#if CONFIG_GPS
    case BST_I2C_GPS:
        len = sizeof(bstBattery);
        memcpy(buffer, (uint8_t *)&bstGpsPosition, len);
        break;
#endif
    default:
        return;
    }
    buffer[len] = getCrc(buffer, len);
    Wire.write(buffer, len);
#ifdef DEBUG
    for (int i = 0; i <= len; i++)
    {
        DEBUG_SERIAL.print(buffer[i], HEX);
        DEBUG_SERIAL.print(" ");
    }
    DEBUG_SERIAL.println();
#endif
}

void Bst::begin()
{
    pinMode(LED_BUILTIN, OUTPUT);
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega32U4__)
#if CONFIG_GPS
    addressMask |= BST_I2C_GPS;
    GPS_SERIAL.begin(GPS_BAUD_RATE);
    GPS_SERIAL.setTimeout(BN220_TIMEOUT);
#endif
#if CONFIG_VOLTAGE1 || CONFIG_CURRENT
    addressMask |= BST_I2C_BATTERY;
#endif
    addressMask = 0xFF;
    Wire.begin(addressMask);
    Wire.onRequest(i2c_request_handler);
    TWAMR = addressMask << 1;
#endif
#if defined(__MKL26Z64__) || defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
    Wire.begin(BST_AIRSPEED);
    Wire.onRequest(i2c_request_handler);
    I2C0_RA = BST_RPM_VOLT_TEMP << 1;
#endif
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
#if defined(SIM_RX)
    static uint32_t timestamp = 0;
    if (millis() - timestamp > 94)
    {
        i2c_request_handler();
        timestamp = millis();
    }
#endif
}

uint8_t Bst::getCrc(uint8_t *buffer, uint8_t len)
{
    uint8_t crc = 0;
    for (uint8_t i = 1; i < len; i++)
        crc =+ getByteCrc(buffer[i]);
    return crc;
}

uint8_t Bst::getByteCrc(uint8_t data)
{
    bool flag;
    uint8_t polynomial = 0xD5;
    uint8_t crc;
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
    return crc;
}