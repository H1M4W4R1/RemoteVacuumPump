#ifndef PUMPACTIVATIONSTATUSCALLBACKS_H
#define PUMPACTIVATIONSTATUSCALLBACKS_H
#include <Arduino.h>
#include <BLECharacteristic.h>

#include "../sys_runtime.h"

class PumpActivityStatusCharacteristicCallbacks final : public BLECharacteristicCallbacks
{
    virtual void onWrite(BLECharacteristic* pCharacteristic) override
    {
        const std::string textValue = pCharacteristic->getValue();
        const uint8_t value = textValue.at(0);

        if (value)
            sys_runtime_activate_pump();
        else sys_runtime_deactivate_pump();
    }
};


#endif
