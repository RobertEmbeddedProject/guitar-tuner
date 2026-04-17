#include "IO_init.h"
#include "python_graph.h"
#include "main.h"
/*
#include <task.h>               //vTask APIs
#include <SEGGER_SYSVIEW.h>
#include <SEGGER_RTT.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>             //provides ptrs, array size utilities
#include <FreeRTOS.h>
#include <semphr.h>             //semaphore APIs
*/


TIM_HandleTypeDef htim2;

void MX_TIM2_Init(void)
{
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    __HAL_RCC_TIM2_CLK_ENABLE();

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 15;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 124;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
}






int main(void)
{
    HAL_Init();
    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    UART6_Init();
    MX_ADC1_Init();

    //From the HAL:
    //HAL_ADC_Start_DMA(ADC_HandleTypeDef* hadc, uint32_t* pData, uint32_t Length);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_dma_buf, 2*SAMPLES);
    //because initialization selects circular, this data will be updated continuously

    MX_TIM2_Init();
    HAL_TIM_Base_Start(&htim2);

    /*For Serial Monitoring
    const char *msg1 = "heart\r\n";
    const char *msg2 = "beat\r\n";
    */

    
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    //HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init);


    while (1)
    {

        python_graph();

        /* Serial Print to Putty for monitor troubleshooting
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_SET);
        HAL_Delay(200);
        HAL_UART_Transmit(&huart6, (uint8_t*)msg1, strlen(msg1), HAL_MAX_DELAY);
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_RESET);
        HAL_Delay(200);
        HAL_UART_Transmit(&huart6, (uint8_t*)msg2, strlen(msg2), HAL_MAX_DELAY);
        */
    }
}


