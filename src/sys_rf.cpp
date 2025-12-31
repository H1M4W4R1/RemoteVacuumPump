#include "sys_rf.h"

#include "sys_runtime.h"

RCSwitch rxSwitch = RCSwitch();
int32_t rf_cooldown_timer; // signed to prevent issues

void sys_rf_init()
{
    // Configure RF RX
    constexpr int32_t interruptPin = digitalPinToInterrupt(RF_GPIO_PIN);
    rxSwitch.enableReceive(interruptPin);  // We need to convert GPIO0 to IRQ
    rxSwitch.setReceiveTolerance(25);

    Serial.println("[Device] RF initialized.");
}


void handle_rf_command()
{
    if (rf_cooldown_timer > 0) return;

    const uint32_t receivedValue = rxSwitch.getReceivedValue();
    const uint32_t detectedProtocol = rxSwitch.getReceivedProtocol();

    if (detectedProtocol != RF_PROTOCOL) return;

    if (receivedValue == RF_BUTTON_A) // Activate / deactivate pump when A is pressed
    {
        if (!is_pump_active)
            sys_runtime_activate();
        else
            sys_runtime_deactivate_pump();

        rf_cooldown_timer = RF_COOLDOWN_TIME;
    }
    else if(receivedValue == RF_BUTTON_B) // Change valve when B is pressed
    {
        // Disable valve remote change if not allowed to prevent air release using RF
        // when RF is used only to activate the device
        if (is_valve_remote_change_disabled) return;

        sys_runtime_switch_valve();
        rf_cooldown_timer = RF_COOLDOWN_TIME;
    }
}

void sys_rf_loop(const uint32_t deltaTime)
{
    // Decrease and check cooldown
    if (rf_cooldown_timer > 0) rf_cooldown_timer -= deltaTime;

    if (rxSwitch.available()) {
        handle_rf_command();
        rxSwitch.resetAvailable();
    }
}