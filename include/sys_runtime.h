#ifndef SYS_RUNTIME_H
#define SYS_RUNTIME_H
#include "Arduino.h"


// GPIO
#define GPIO_VALVE_PIN 7
#define GPIO_PUMP_PIN  9

// Safety
#define MAX_SAFE_TIME     (8 * 3600) // 8 hours for safety

extern uint32_t runtime_session_timer_seconds;
extern uint32_t runtime_pump_timer_seconds;

extern uint32_t expected_session_time;
extern uint32_t expected_pumping_time;

extern uint8_t is_pump_active;
extern uint8_t is_valve_locked;
extern uint8_t is_valve_remote_change_disabled;

void sys_runtime_init();
void sys_runtime_loop(uint32_t deltaTime);

void sys_runtime_activate();
void sys_runtime_activate_pump();
void sys_runtime_deactivate_pump();
void sys_runtime_activate_valve();
void sys_runtime_deactivate_valve();
void sys_runtime_switch_valve();
void sys_runtime_session_end();

#endif //SYS_RUNTIME_H
