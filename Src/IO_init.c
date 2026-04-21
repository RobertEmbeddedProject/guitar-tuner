#include "IO_init.h"
#include "python_graph.h"

UART_HandleTypeDef huart6;
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
TIM_HandleTypeDef htim2;


void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

                 //Port                   //Struct
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    //combine each 0b000.. bitwise operator to one mapping before initialization
    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 |
                        GPIO_PIN_9 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13| GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_14 | GPIO_PIN_9;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}

static const LED tuning_led[19] = {
    {GPIOD, GPIO_PIN_7},   // P0 Red1
    {GPIOD, GPIO_PIN_6},   // P1 Red2
    {GPIOD, GPIO_PIN_5},   // P2 Red3
    {GPIOD, GPIO_PIN_4},   // P3 Red4
    {GPIOE, GPIO_PIN_2},   // P4 Yel1
    {GPIOE, GPIO_PIN_4},   // P5 Yel2
    {GPIOE, GPIO_PIN_5},   // P6 Yel3
    {GPIOE, GPIO_PIN_6},   // P7 Yel4
    {GPIOF, GPIO_PIN_13},  // P8 Grn1
    {GPIOE, GPIO_PIN_9},   // P9 Grn2
    {GPIOE, GPIO_PIN_11},  // P10 Grn3
    {GPIOE, GPIO_PIN_13},  // P11 Yel5
    {GPIOF, GPIO_PIN_15},  // P12 Yel6
    {GPIOG, GPIO_PIN_14},  // P13 Yel7
    {GPIOG, GPIO_PIN_9},   // P14 Yel8
    {GPIOE, GPIO_PIN_14},  // P15 Red5
    {GPIOE, GPIO_PIN_15},  // P16 Red6
    {GPIOB, GPIO_PIN_10},  // P17 Red7
    {GPIOB, GPIO_PIN_11}   // P18 Red8
};

static const LED status_led[4] = {
    {GPIOD, GPIO_PIN_3},   // P0 Wht1
    {GPIOF, GPIO_PIN_8},   // P1 Wht2
    {GPIOF, GPIO_PIN_14},  // P2 Wht3
    {GPIOE, GPIO_PIN_12},  // P3 Wht4
};

void LED_ON(uint16_t position){
    HAL_GPIO_WritePin(tuning_led[position].port, tuning_led[position].pin, GPIO_PIN_SET);
}

void LED_OFF(uint16_t position){
    HAL_GPIO_WritePin(tuning_led[position].port, tuning_led[position].pin, GPIO_PIN_RESET);
}

void blink_status(){
    HAL_GPIO_WritePin(status_led[0].port, status_led[0].pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(status_led[1].port, status_led[1].pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(status_led[2].port, status_led[2].pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(status_led[3].port, status_led[3].pin, GPIO_PIN_SET);
    HAL_Delay(200); //vTaskDelay(20 / portTICK_PERIOD_MS);
    HAL_GPIO_WritePin(status_led[0].port, status_led[0].pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(status_led[1].port, status_led[1].pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(status_led[2].port, status_led[2].pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(status_led[3].port, status_led[3].pin, GPIO_PIN_RESET);
    HAL_Delay(200); //vTaskDelay(20 / portTICK_PERIOD_MS);
}

void UART6_Init(void)
{
    __HAL_RCC_USART6_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    huart6.Instance = USART6;
    huart6.Init.BaudRate = 460800;
    huart6.Init.WordLength = UART_WORDLENGTH_8B;
    huart6.Init.StopBits = UART_STOPBITS_1;
    huart6.Init.Parity = UART_PARITY_NONE;
    huart6.Init.Mode = UART_MODE_TX;
    huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart6.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart6) != HAL_OK)
    {
        Error_Handler();
    }
}

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

void MX_ADC1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    ADC_ChannelConfTypeDef sConfig = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE();

    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    hdma_adc1.Instance = DMA2_Stream0;
    hdma_adc1.Init.Channel = DMA_CHANNEL_0;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
    hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
    {
        Error_Handler();
    }

    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T2_TRGO;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = ENABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;

    __HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc1);

    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }

    sConfig.Channel = ADC_CHANNEL_3;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

void DMA2_Stream0_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_adc1);
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        adc_half_ready = 1;
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        adc_full_ready = 1;
    }
}


//from CubeMX
void Error_Handler(void)
{
    while (1)
    {
    }
}

void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file;
    (void)line;

    while (1)
    {
    }
}