#ifndef SESSIONTIMESTATUSCALLBACKS_H
#define SESSIONTIMESTATUSCALLBACKS_H
#include <Arduino.h>
#include <BLECharacteristic.h>

#include "sys_bluetooth.h"
#include "../sys_runtime.h"

class CurrentSessionTimeCharacteristicCallbacks final : public BLECharacteristicCallbacks
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

        runtime_session_timer_seconds = value;
        sys_bluetooth_update_current_session_time();
    }
};


#endif
