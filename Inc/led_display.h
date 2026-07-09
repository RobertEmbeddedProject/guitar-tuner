#pragma once
#include <stm32f7xx_hal.h>

typedef struct{GPIO_TypeDef *port; uint16_t pin;} LED;

uint32_t LED_tuning_ind(float cents);
void LED_ON(uint16_t position);
void LED_OFF(uint16_t position);
void LED_all_off();
void blink_status();


