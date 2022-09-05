#pragma once

#include <stdint.h>

typedef struct {
    double temperature;
    double humidity;
} AirData;

typedef enum {
    TEMPRES_14BIT = 0,
    TEMPRES_11BIT = 1 << 10
} TempMeasureResolution;

typedef enum {
    HUMRES_14BIT = 0,
    HUMRES_11BIT = 1 << 8,
    HUMRES_8BIT = 1 << 9
} HumidityMeasureResolution;

class HDC1080 {
public:
    HDC1080();
    uint16_t readManufacturerId();
    uint16_t readDeviceId();
    uint64_t readDeviceSerialId();
    double measureTemperature(TempMeasureResolution tempResolution = TempMeasureResolution::TEMPRES_14BIT);
    double measureHumidity(HumidityMeasureResolution humResolution = HumidityMeasureResolution::HUMRES_14BIT);
    AirData measureTempAndHum(TempMeasureResolution tempResolution = TempMeasureResolution::TEMPRES_14BIT, HumidityMeasureResolution humResolution = HumidityMeasureResolution::HUMRES_14BIT);
    void setHeater(bool enable);

private:
    bool deviceAvailable;
    uint16_t manufacturerId;
    uint16_t readMsg(uint8_t registerPointer, uint32_t waitTime = 0);
    void writeReg(uint16_t value);
    // void writeReg(uint16_t value16);
    void writeReg(std::vector<uint16_t> values);
};