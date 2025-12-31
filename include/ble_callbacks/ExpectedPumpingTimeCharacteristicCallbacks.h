#ifndef EXPECTEDPUMPINGTIMEACTIVATIONCALLBACKS_H
#define EXPECTEDPUMPINGTIMEACTIVATIONCALLBACKS_H
#include <Arduino.h>
#include <BLECharacteristic.h>

#include "../sys_runtime.h"

class ExpectedPumpingTimeCharacteristicCallbacks final : public BLECharacteristicCallbacks
{
    virtual void onWrite(BLECharacteristic* pCharacteristic) override
    {
        const std::string textValue = pCharacteristic->getValue();

        // Copy data to value nicely :)
        uint32_t          value;
        uint8_t*          data = reinterpret_cast<uint8_t*>(&value);
        data[0]                = textValue[0];
        data[1]                = textValue[1];
        data[2]                = textValue[2];
        data[3]                = textValue[3];

        expected_pumping_time = value;
    }
};


#endif
