#include <Wire.h>
#include <Arduino.h>

#include "hdc1080.h"

constexpr uint8_t address = 0b1000000; // HDC1080 has fixed i2c address.
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

HDC1080::HDC1080(uint8_t pinSDA, uint8_t pinSCL)
    : pinSDA(pinSDA)
    , pinSCL(pinSCL) {
}

uint16_t HDC1080::readManufacturerId() {
    uint16_t manId = 0;
    manId = readMsg(registerManufacturerId);
    Serial.print("manufacturer id: ");
    Serial.println(manId);

    return manId;
}

uint16_t HDC1080::readDeviceId() {
    uint16_t deviceId = 0;
    deviceId = readMsg(registerDeviceId);
    Serial.print("manufacturer id: ");
    Serial.println(deviceId);

    return deviceId;
}

uint64_t HDC1080::readDeviceSerialId() {
    uint64_t deviceSerial = 0;
    uint16_t first = readMsg(registerDeviceSerial[0]);
    uint16_t mid = readMsg(registerDeviceSerial[1]);
    uint16_t last = readMsg(registerDeviceSerial[2]);
    deviceSerial = (uint64_t)first << 32 | mid << 16 | last;
    Serial.print("serial id: ");
    // Serial.println(first);
    Serial.println(deviceSerial);
    return deviceSerial;
}

uint16_t HDC1080::readMsg(uint8_t registerPointer) {
    Msg16 msg = {};
    Wire.beginTransmission(address);
    Wire.write(registerPointer);
    Wire.endTransmission();
    uint8_t nrBytes = Wire.requestFrom(address, lengthMsg);
    if (nrBytes == lengthMsg) {
        msg.msgRaw.dataMSB = Wire.read();
        msg.msgRaw.dataLSB = Wire.read();
    }

    return msg.msgFull;
}