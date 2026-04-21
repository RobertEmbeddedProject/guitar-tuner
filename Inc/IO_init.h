#pragma once
#include <stm32f7xx_hal.h>

extern UART_HandleTypeDef huart6;
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim2;

void UART6_Init(void);
void MX_ADC1_Init(void);
void MX_TIM2_Init(void);
void LED_Init(void);

void LED_ON(uint16_t position);
void LED_OFF(uint16_t position);
void blink_status();

void Error_Handler(void);

typedef struct{GPIO_TypeDef *port; uint16_t pin;} LED;
