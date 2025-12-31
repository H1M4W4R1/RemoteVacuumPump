#ifndef VALVELOCKALLOWEDCALLBACKS_H
#define VALVELOCKALLOWEDCALLBACKS_H
#include <Arduino.h>
#include <BLECharacteristic.h>

#include "../sys_runtime.h"

class ValveLockAllowedCharacteristicCallbacks final : public BLECharacteristicCallbacks
{
    virtual void onWrite(BLECharacteristic* pCharacteristic) override
    {
        const std::string textValue = pCharacteristic->getValue();
        const uint8_t value = textValue.at(0);

        is_valve_remote_change_disabled = value;
    }
};


#endif
