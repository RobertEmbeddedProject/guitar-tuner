#include "IO_init.h"
#include "yin_tuner.h"
#include "led_display.h"
#include <stdio.h>
#include <math.h>
#include "main.h"
//#include "python_graph.h"

static int FindClosestGuitarString(float freq_hz);

#define SAMPLE_RATE_HZ 20000.0f
#define DMA_HALF_SAMPLES 2048
#define DMA_TOTAL_SAMPLES (2 * DMA_HALF_SAMPLES)
uint16_t adc_dma_buf[DMA_TOTAL_SAMPLES];

typedef struct
{
    const char *name;
    float freq_hz;
} GuitarString_t;

//extern cents = 1200.0f * log2f(measured_hz / target_hz);
static const GuitarString_t strings[] =
{
    {"Low E", 82.41f},
    {"A",     110.00f},
    {"D",     146.83f},
    {"G",     196.00f},
    {"B",     246.94f},
    {"High E",329.63f}
};


int main(void)
{
    HAL_Init();
    SystemClock_Config(); //added for increasing clock to 216Mhz

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

    volatile uint32_t next_LED = 0;
    //volatile uint32_t prev_LED = 0;

    // Main Loop
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
                int string_index = FindClosestGuitarString(r.freq_hz);

                float target_hz = strings[string_index].freq_hz;

                float cents =
                    1200.0f * log2f(r.freq_hz / target_hz);

                next_LED = LED_tuning_ind(cents);

                YIN_print(r, cents, next_LED);

                LED_all_off();
                LED_ON(next_LED);
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
                int string_index = FindClosestGuitarString(r.freq_hz);

                float target_hz = strings[string_index].freq_hz;

                float cents =
                    1200.0f * log2f(r.freq_hz / target_hz);

                next_LED = LED_tuning_ind(cents);

                YIN_print(r, cents, next_LED);

                LED_all_off();
                LED_ON(next_LED);
            }
            else
            {
                LED_all_off();
            }
        }
    }
}

static int FindClosestGuitarString(float freq_hz)
{
    int best_index = 0;
    float best_error = 999999.0f;

    for (int i = 0; i < 6; i++)
    {
        float error = fabsf(
            1200.0f * log2f(freq_hz / strings[i].freq_hz)
        );

        if (error < best_error)
        {
            best_error = error;
            best_index = i;
        }
    }

    return best_index;
}


