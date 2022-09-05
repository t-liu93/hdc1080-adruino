#include <Wire.h>
#include <Arduino.h>

#include "hdc1080.h"

constexpr uint8_t address = 0b1000000; // HDC1080 has fixed i2c address.
constexpr uint8_t registerConfig = 0x02;
constexpr uint8_t registerTempRead = 0x00;
constexpr uint8_t registerHumiRead = 0x01;
constexpr uint8_t registerManufacturerId = 0xFE;
constexpr uint8_t registerDeviceId = 0xFF;
constexpr uint8_t registerDeviceSerial[] = {0xFB, 0xFC, 0xFD};
constexpr uint8_t lengthMsg = 2;

typedef struct {
    uint8_t dataLSB;
    uint8_t dataMSB;
} Msg;

typedef union {
    Msg msgRaw;
    uint16_t msgFull;
} Msg16;

HDC1080::HDC1080() {
    manufacturerId = readManufacturerId();
}

uint16_t HDC1080::readManufacturerId() {
    uint16_t manId = 0;
    manId = readMsg(registerManufacturerId);

    return manId;
}

uint16_t HDC1080::readDeviceId() {
    uint16_t deviceId = 0;
    deviceId = readMsg(registerDeviceId);

    return deviceId;
}

uint64_t HDC1080::readDeviceSerialId() {
    uint64_t deviceSerial = 0;
    uint16_t first = readMsg(registerDeviceSerial[0]);
    uint16_t mid = readMsg(registerDeviceSerial[1]);
    uint16_t last = readMsg(registerDeviceSerial[2]);
    deviceSerial = (uint64_t)first << 32 | mid << 16 | last;
    return deviceSerial;
}

double HDC1080::measureTemperature(TempMeasureResolution tempResolution) {
    uint32_t delayTime = 0;
    switch (tempResolution) {
        case TempMeasureResolution::TEMPRES_14BIT:
            delayTime = 7;
            break;
        case TempMeasureResolution::TEMPRES_11BIT:
            delayTime = 4;
            break;
    }
    uint16_t configValue = tempResolution;
    Wire.beginTransmission(address);
    Wire.write(registerConfig);
    Wire.write(configValue);
    Wire.endTransmission();
    uint16_t tempRaw = readMsg(registerTempRead, delayTime);
    return ((double)tempRaw / pow(2, 16)) * 165 - 40;
}

double HDC1080::measureHumidity(HumidityMeasureResolution humResolution) {
    uint32_t delayTime = 0;
    switch (humResolution) {
        case HumidityMeasureResolution::HUMRES_14BIT:
            delayTime = 7;
            break;
        case HumidityMeasureResolution::HUMRES_11BIT:
            delayTime = 4;
            break;
        case HumidityMeasureResolution::HUMRES_8BIT:
            delayTime = 3;
            break;
    }
    uint16_t configValue = humResolution;
    Wire.beginTransmission(address);
    Wire.write(registerConfig);
    Wire.write(configValue);
    Wire.endTransmission();
    uint16_t humRaw = readMsg(registerHumiRead, delayTime);
    return ((double)humRaw / pow(2, 16));
}

AirData HDC1080::measureTempAndHum(TempMeasureResolution tempResolution, HumidityMeasureResolution humResolution) {
    AirData retval = {0, 0};
    // Use two single measurement
    retval.temperature = measureTemperature(tempResolution);
    retval.humidity = measureHumidity(humResolution);

    return retval;
}

void HDC1080::setHeater(bool enable) {
    uint16_t configValue = enable ? 1 << 13 : 0;
    Wire.beginTransmission(address);
    Wire.write(registerConfig);
    Wire.write(configValue);
    Wire.endTransmission();
}

uint16_t HDC1080::readMsg(uint8_t registerPointer, uint32_t waitTime) {
    Msg16 msg = {};
    Wire.beginTransmission(address);
    Wire.write(registerPointer);
    Serial.println(Wire.endTransmission());
    uint8_t nrBytes = Wire.requestFrom(address, lengthMsg);
    Serial.println(nrBytes);
    if (nrBytes == lengthMsg) {
        msg.msgRaw.dataMSB = Wire.read();
        msg.msgRaw.dataLSB = Wire.read();
    }

    return msg.msgFull;
}
