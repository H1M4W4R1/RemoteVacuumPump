#ifndef SYS_BLUETOOTH_H
#define SYS_BLUETOOTH_H

#include "sys_runtime.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define SERVICE_UUID_CORE_FUNC                          "ae615e5d-b4be-428e-8ff9-9348c929a36e"

#define CHARACTERISTIC_SESSION_EXPECTED_TIME            "ae615c93-0000-4b7e-95ea-38792724bf8f"
#define CHARACTERISTIC_PUMP_ACTIVITY_STATUS             "ae615056-0001-4c20-a036-408083930b06"
#define CHARACTERISTIC_VALVE_LOCK_STATUS                "ae615d1c-0002-440c-8e7a-a976b1e560bc"
#define CHARACTERISTIC_CURRENT_SESSION_TIME             "ae61502e-0003-4bd1-8440-408169e0323a"
#define CHARACTERISTIC_VALVE_LOCK_ALLOWED               "ae615096-0004-49ff-9ab0-fa194359595a"
#define CHARACTERISTIC_PUMP_EXPECTED_TIME               "ae6152b7-0005-49c3-975e-42c7fa2e14c9"
#define CHARACTERISTIC_CURRENT_PUMPING_TIME             "ae6153c8-0006-4dd3-b764-80715dde7cab"

void sys_bluetooth_init();
void sys_bluetooth_update_pump_status();
void sys_bluetooth_update_valve_status();
void sys_bluetooth_update_current_session_time();
void sys_bluetooth_update_current_pump_time();

#endif //SYS_BLUETOOTH_H
