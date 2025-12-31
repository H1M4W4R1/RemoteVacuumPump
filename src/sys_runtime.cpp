#include "sys_runtime.h"

#include "sys_bluetooth.h"

uint32_t expected_pumping_time = MAX_SAFE_TIME;
uint32_t expected_session_time = MAX_SAFE_TIME;
uint8_t is_pump_active;
uint8_t is_valve_locked;
uint8_t is_valve_remote_change_disabled;


uint32_t runtime_session_timer_seconds = 0;
uint32_t runtime_pump_timer_seconds = 0;

uint32_t runtime_session_timer_millis = 0;
uint32_t runtime_pump_timer_millis = 0;

void sys_runtime_init()
{
    pinMode(GPIO_VALVE_PIN, OUTPUT);
    pinMode(GPIO_PUMP_PIN, OUTPUT);
    digitalWrite(GPIO_VALVE_PIN, LOW);
    digitalWrite(GPIO_PUMP_PIN, LOW);

    Serial.println("[Device] Runtime initialized.");
}

void sys_runtime_loop(const uint32_t deltaTime)
{
    // Handle pump controls
    if (is_pump_active)
    {
        runtime_pump_timer_millis += deltaTime;
        while (runtime_pump_timer_millis > 1000)
        {
            runtime_pump_timer_millis -= 1000;
            runtime_pump_timer_seconds++;
            sys_bluetooth_update_current_pump_time();
        }

        if (runtime_pump_timer_seconds >= expected_pumping_time)
            sys_runtime_deactivate_pump();
    }

    // Handle session controls
    if (is_valve_locked)
    {
        // Update timer if more than a second has passed
        runtime_session_timer_millis += deltaTime;
        while (runtime_session_timer_millis > 1000)
        {
            runtime_session_timer_millis -= 1000;
            runtime_session_timer_seconds++;
            sys_bluetooth_update_current_session_time();
        }

        // Handle session end and failsafe
        if (runtime_session_timer_seconds >= expected_session_time)
        {
            sys_runtime_session_end();
        }
        else if (runtime_session_timer_seconds >= MAX_SAFE_TIME)
        {
            Serial.println("[Session] Failsafe detected.");

            // ReSharper disable once CppDFAEndlessLoop
            while (true)
            {
                sys_runtime_session_end();
            }
        }
    }
}

void sys_runtime_activate()
{
    Serial.println("[Device] Device activated.");
    sys_runtime_activate_pump();
    sys_runtime_activate_valve();
}

void sys_runtime_activate_pump()
{
    if (is_pump_active) return; // To prevent potential timer resets
    Serial.println("[Pump] Pump enabled.");
    is_pump_active = true;
    digitalWrite(GPIO_PUMP_PIN, HIGH);
    sys_bluetooth_update_pump_status();
}

void sys_runtime_deactivate_pump()
{
    Serial.println("[Pump] Pump disabled.");
    is_pump_active = false;
    digitalWrite(GPIO_PUMP_PIN, LOW);
    runtime_pump_timer_millis = 0;
    runtime_pump_timer_seconds = 0;
    sys_bluetooth_update_pump_status();
    sys_bluetooth_update_current_pump_time();
}

void sys_runtime_activate_valve()
{
    Serial.println("[Valve] Valve locked.");
    is_valve_locked = true;
    digitalWrite(GPIO_VALVE_PIN, HIGH);
    sys_bluetooth_update_valve_status();
}

void sys_runtime_deactivate_valve()
{
    Serial.println("[Valve] Valve unlocked.");
    is_valve_locked = false;
    digitalWrite(GPIO_VALVE_PIN, LOW);
    sys_bluetooth_update_valve_status();
}

void sys_runtime_switch_valve()
{
    if (is_valve_locked) sys_runtime_deactivate_valve();
    else sys_runtime_activate_valve();
}

void sys_runtime_session_end()
{
    Serial.println("[Session] Session ended.");
    sys_runtime_deactivate_pump();
    sys_runtime_deactivate_valve();
    runtime_session_timer_millis = 0;
    runtime_session_timer_seconds = 0;
}