#pragma once

#include <stdint.h>

class HDC1080 {
public:
    HDC1080(uint8_t pinSDA, uint8_t pinSCL);
    uint16_t readManufacturerId();
    uint16_t readDeviceId();
    uint64_t readDeviceSerialId();

private:
    uint16_t readMsg(uint8_t registerPointer);
    uint8_t pinSDA;
    uint8_t pinSCL;
};