#include "IO_init.h"
#include "yin_tuner.h"
#include <stdio.h>
#include "main.h"
//#include "python_graph.h"

#define SAMPLE_RATE_HZ 20000.0f
#define DMA_HALF_SAMPLES 2048
#define DMA_TOTAL_SAMPLES (2 * DMA_HALF_SAMPLES)
uint16_t adc_dma_buf[DMA_TOTAL_SAMPLES];

int main(void)
{
    HAL_Init();

    //Enable peripheral clocks
    __HAL_RCC_DMA2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    //IO_init drivers
    UART6_Init();
    MX_TIM2_Init();
    MX_ADC1_Init();
    LED_Init();
    
    //DMA Driver + Error Handler
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_dma_buf, DMA_TOTAL_SAMPLES) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_Base_Start(&htim2) != HAL_OK)
    {
        Error_Handler();
    }

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    //HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init);


    //Main Loop
    while (1)
    {
        if (adc_half_ready)
        {
            adc_half_ready = 0;

            YIN_Result_t r = YIN_DetectPitch(&adc_dma_buf[0],
                                            DMA_HALF_SAMPLES,
                                            SAMPLE_RATE_HZ);
            PrintPitchResult(r);
        }

        if (adc_full_ready)
        {
            adc_full_ready = 0;

            YIN_Result_t r = YIN_DetectPitch(&adc_dma_buf[DMA_HALF_SAMPLES],
                                            DMA_HALF_SAMPLES,
                                            SAMPLE_RATE_HZ);
            PrintPitchResult(r);
        }
    }
}

//For Troubleshooting purposes
void test_print(void){
        //Print graph to serial_print.py
        //python_graph();

        /* PuTTy COM7 printing. Not useful unless printing ASCII.
        int len = snprintf(msg, sizeof(msg),
        "%u,%u,%u,%u\r\n",
        adc_dma_buf[0],
        adc_dma_buf[1],
        adc_dma_buf[2],
        adc_dma_buf[3]);
        HAL_UART_Transmit(&huart6, (uint8_t*)msg, len, HAL_MAX_DELAY);
        //Port COM7, BAUD 460800
        HAL_Delay(200);
        */

        /*  LED Cycle for all outputs
        for(int i=0; i<19; i++){
            LED_ON(i);
            HAL_Delay(100);
            LED_OFF(i);
        }
        */

        /* (OLD) Serial Print to Putty for monitor troubleshooting
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_SET);
        HAL_Delay(200);
        HAL_UART_Transmit(&huart6, (uint8_t*)msg1, strlen(msg1), HAL_MAX_DELAY);
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_RESET);
        HAL_Delay(200);
        HAL_UART_Transmit(&huart6, (uint8_t*)msg2, strlen(msg2), HAL_MAX_DELAY);
        */
}