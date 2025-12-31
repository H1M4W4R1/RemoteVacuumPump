#ifndef SYS_RF_H
#define SYS_RF_H

#include <RCSwitch.h>

#define RF_GPIO_PIN         1
#define RF_PROTOCOL         1
#define RF_BUTTON_A         7300524
#define RF_BUTTON_B         7300528
#define RF_COOLDOWN_TIME    1000

void sys_rf_init();
void sys_rf_loop(uint32_t deltaTime);

#endif //SYS_RF_H
