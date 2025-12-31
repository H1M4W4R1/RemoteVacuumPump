#include <Arduino.h>

#include "sys_bluetooth.h"
#include "sys_rf.h"

uint32_t globalLastTickTime;

void setup() {
  // Setup communication
  Serial.begin(9600);
  Serial.println("Configuring...");

  sys_runtime_init();
  sys_rf_init();
  sys_bluetooth_init();

  Serial.println("Initialization complete.");
}

void loop()
{
  // Compute delta time to prevent update issues
  const uint32_t current_time_millis = millis();
  const uint32_t deltaTime = current_time_millis - globalLastTickTime;

  sys_runtime_loop(deltaTime);
  sys_rf_loop(deltaTime);
  globalLastTickTime = current_time_millis;
}
