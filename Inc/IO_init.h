#pragma once
#include <stm32f7xx_hal.h>

extern UART_HandleTypeDef huart6;
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim2;
extern volatile uint8_t adc_half_ready;
extern volatile uint8_t adc_full_ready;

void SystemClock_Config(void);
void LED_Init(void);
void UART6_Init(void);
void MX_TIM2_Init(void);
void MX_ADC1_Init(void);
void Error_Handler(void);

