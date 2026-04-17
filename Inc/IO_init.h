#include <stm32f7xx_hal.h>      //ST HAL

extern UART_HandleTypeDef huart6;
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

void UART6_Init(void);
void MX_ADC1_Init(void);
void Error_Handler(void);




