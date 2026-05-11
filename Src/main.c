#include "IO_init.h"
#include "yin_tuner.h"
#include "led_display.h"
#include <stdio.h>
#include "main.h"
//#include "python_graph.h"

#define SAMPLE_RATE_HZ 20000.0f
#define DMA_HALF_SAMPLES 2048
#define DMA_TOTAL_SAMPLES (2 * DMA_HALF_SAMPLES)
uint16_t adc_dma_buf[DMA_TOTAL_SAMPLES];

void SystemClock_Config(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config(); //added just for increasing clock to 216Mhz

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

    //Temporarily fill test: buffer for LED work
    /*
    for (uint32_t i = 2048; i < 4096; i++)
        {
            adc_dma_buf[i] = 2048 + (i % 243 < 121 ? 400 : -400);
        }
*/

    volatile uint32_t next_LED = 0;
    volatile uint32_t prev_LED = 0;

    //Main Loop
    while (1)
    {
        if (adc_half_ready)
        {
            adc_half_ready = 0;

            YIN_Result_t r = YIN_DetectPitch(&adc_dma_buf[0],
                                            DMA_HALF_SAMPLES,
                                            SAMPLE_RATE_HZ);

            if (r.valid && r.confidence > 0.80f)
            {
                next_LED = LED_tuning_ind(r.cents);
                YIN_print(r, next_LED);

                LED_all_off();
                LED_ON(next_LED);

                prev_LED = next_LED;
            }
            else
            {
                LED_all_off();
            }
        }

        if (adc_full_ready)
        {
            adc_full_ready = 0;

            YIN_Result_t r = YIN_DetectPitch(&adc_dma_buf[DMA_HALF_SAMPLES],
                                            DMA_HALF_SAMPLES,
                                            SAMPLE_RATE_HZ);

            if (r.valid && r.confidence > 0.80f)
            {
                next_LED = LED_tuning_ind(r.cents);
                YIN_print(r, next_LED);

                LED_all_off();
                LED_ON(next_LED);

                prev_LED = next_LED;
            }
            else
            {
                LED_all_off();
            }
        }
    }
}


void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;

    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;

    /*
     * HSI = 16 MHz
     * VCO input = 16 MHz / 8 = 2 MHz
     * VCO output = 2 MHz * 216 = 432 MHz
     * SYSCLK = 432 MHz / 2 = 216 MHz
     */
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 216;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 9;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_HCLK |
        RCC_CLOCKTYPE_PCLK1 |
        RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
    {
        Error_Handler();
    }
}