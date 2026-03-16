#include "sys_bluetooth.h"

#include "../include/ble_callbacks/PumpActivityStatusCharacteristicCallbacks.h"
#include "../include/ble_callbacks/ExpectedSessionTimeCharacteristicCallbacks.h"
#include "../include/ble_callbacks/ValveLockStatusCharacteristicCallbacks.h"
#include "ble_callbacks/CurrentPumpingTimeCharacteristicCallbacks.h"
#include "ble_callbacks/CurrentSessionTimeCharacteristicCallbacks.h"
#include "ble_callbacks/ExpectedPumpingTimeCharacteristicCallbacks.h"
#include "ble_callbacks/ValveLockAllowedCharacteristicCallbacks.h"

class ServerCallbacks : public BLEServerCallbacks {
    void onDisconnect(BLEServer* pServer) override {
        BLEDevice::startAdvertising();
        Serial.println("[Device] Client disconnected, restarting advertising.");
    }
};

// BLE
BLECharacteristic* totalSessionTimeCharacteristic;
BLECharacteristic* valveLockedCharacteristic;
BLECharacteristic* pumpActiveCharacteristic;
BLECharacteristic* currentSessionTimeCharacteristic;

BLECharacteristic* valveRemoteChangeAllowedCharacteristic;
BLECharacteristic* currentPumpingTimeCharacteristic;
BLECharacteristic* expectedPumpingTimeCharacteristic;

void sys_bluetooth_init()
{
    // Setup BLE
    BLEDevice::init("Vacuum Pump Prototype");
    BLEServer*  pServer  = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());
    BLEService* pService = pServer->createService(SERVICE_UUID_CORE_FUNC);

    // Session time characteristic
    BLECharacteristic* pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_SESSION_EXPECTED_TIME, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    pCharacteristic->setCallbacks(new ExpectedSessionTimeCharacteristicCallbacks());
    pCharacteristic->setValue(expected_session_time);

    // Pump activity enable/disable
    pumpActiveCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_PUMP_ACTIVITY_STATUS,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
    pumpActiveCharacteristic->setCallbacks(new PumpActivityStatusCharacteristicCallbacks());
    pumpActiveCharacteristic->setValue(&is_pump_active, 1);

    // Valve open/close
    valveLockedCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_VALVE_LOCK_STATUS,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
    valveLockedCharacteristic->setCallbacks(new ValveLockStatusCharacteristicCallbacks());
    valveLockedCharacteristic->setValue(&is_valve_locked, 1);

    // Current session time used to shorten or enlengthen the session
    currentSessionTimeCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_CURRENT_SESSION_TIME, BLECharacteristic::PROPERTY_READ
                                                | BLECharacteristic::PROPERTY_WRITE
                                                | BLECharacteristic::PROPERTY_NOTIFY);
    currentSessionTimeCharacteristic->setCallbacks(new CurrentSessionTimeCharacteristicCallbacks());
    currentSessionTimeCharacteristic->setValue(runtime_session_timer_seconds);

    // Block for valve RF controls
    valveRemoteChangeAllowedCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_VALVE_LOCK_ALLOWED, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    valveRemoteChangeAllowedCharacteristic->setCallbacks(new ValveLockAllowedCharacteristicCallbacks());
    valveRemoteChangeAllowedCharacteristic->setValue(&is_valve_remote_change_disabled, 1);

    // Expected time for pump operation
    expectedPumpingTimeCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_PUMP_EXPECTED_TIME, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    expectedPumpingTimeCharacteristic->setCallbacks(new ExpectedPumpingTimeCharacteristicCallbacks());
    expectedPumpingTimeCharacteristic->setValue(expected_pumping_time);

    // Current time for pump operations
    currentPumpingTimeCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_CURRENT_PUMPING_TIME, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
    currentPumpingTimeCharacteristic->setCallbacks(new CurrentPumpingTimeCharacteristicCallbacks());
    currentPumpingTimeCharacteristic->setValue(runtime_pump_timer_seconds);

    // Bootstrap
    pService->start();
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID_CORE_FUNC);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();

    Serial.println("[Device] BLE initialized.");
}

void sys_bluetooth_update_pump_status()
{
    pumpActiveCharacteristic->setValue(&is_pump_active, 1);
    pumpActiveCharacteristic->notify();
}

void sys_bluetooth_update_valve_status()
{
    valveLockedCharacteristic->setValue(&is_valve_locked, 1);
    valveLockedCharacteristic->notify();
}

void sys_bluetooth_update_current_session_time()
{
    currentSessionTimeCharacteristic->setValue(runtime_session_timer_seconds);
    currentSessionTimeCharacteristic->notify();
}

void sys_bluetooth_update_current_pump_time()
{
    currentPumpingTimeCharacteristic->setValue(runtime_pump_timer_seconds);
    currentPumpingTimeCharacteristic->notify();
}
