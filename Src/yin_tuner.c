#include "yin_tuner.h"
#include "IO_init.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_SAMPLES 2048
#define YIN_THRESHOLD 0.15f

static float x[MAX_SAMPLES];
static float diff[MAX_SAMPLES / 2];
static float normdiff[MAX_SAMPLES / 2];

YIN_Result_t YIN_DetectPitch(const uint16_t *adc_buf, uint32_t n, float sample_rate_hz)
{
    //initialize
    YIN_Result_t result = {0};

    #define MIN_FREQ 61.0f
    #define MAX_FREQ 412.0f
    uint32_t min_tau = (uint32_t)(sample_rate_hz / MAX_FREQ);
    uint32_t max_tau = (uint32_t)(sample_rate_hz / MIN_FREQ); 

    //limit check. 512 is min for good data (2 cycles)
    if(n > MAX_SAMPLES){
        n = MAX_SAMPLES;
    }
    if(n < 512){
        return result;
    }

    //limit check; don't search for periods longer than half of window
    if (max_tau >= (n / 2)){
        max_tau = (n / 2) - 1;
    }
    if (min_tau < 2){
        min_tau = 2;
    }

    //remove DC offset of hardware voltage bias
    for (uint32_t i = 0; i < n; i++)
    {
        x[i] = (float)adc_buf[i] - 2048;
    }

    //Step 2 of Yin Method: Difference Function
    for(uint32_t tau = 0; tau <= max_tau; tau++)
    {
        float sum = 0.0f;
        for(uint32_t i = 0; i < n - tau; i++)
        {
            float d = x[i] - x[i + tau];
            sum += d * d;
        }

        diff[tau] = sum;
    }

    //Step 3: Cumulative Mean Normalized Difference Function
    //normdiff(τ) = diff(τ) / ( (1/τ) * Σ_{j=1..τ} diff(j) )
    normdiff[0] = 1.0f;

    float running_sum = 0.0f;

    for(uint32_t tau = 1; tau <= max_tau; tau++)
    {
        //Σ_{j=1..τ} diff(j)
        running_sum += diff[tau];
        if (running_sum > 0.0f)
            {
                normdiff[tau] = diff[tau] * (float)tau / running_sum;
            }
            else
            {
                normdiff[tau] = 1.0f;
                //give bogus value so 0 Tau never gets picked
            }
    }

    //Step 4: Absolute Threshold
    //Finding first Tau below threshold
    uint32_t best_tau = 0;
    for (uint32_t tau = min_tau; tau <= max_tau; tau++)
    {
        if (normdiff[tau] < YIN_THRESHOLD)
        {
            // Walk to local minimum.
            while ((tau + 1 <= max_tau) && (normdiff[tau + 1] < normdiff[tau]))
            {
                tau++;
            }

            best_tau = tau;
            break;
        }
    }
    if (best_tau == 0)
    {
        result.valid = 0;
        result.confidence = 0.0f;
        return result;
    }

    float refined_tau = (float)best_tau;

    if (best_tau > 1 && best_tau < max_tau)
    {
        refined_tau = parab_interpolation(normdiff, best_tau);
    }

    if (refined_tau <= 0.0f)
    {
        return result;
    }

    result.freq_hz = sample_rate_hz / refined_tau;
    result.confidence = 1.0f - normdiff[best_tau];

    if (result.confidence < 0.0f)
    {
        result.confidence = 0.0f;
    }

    if (result.confidence > 1.0f)
    {
        result.confidence = 1.0f;
    }

    result.valid = 1;

    return result;
}

//Step 5 of Yin Method: Parabolic Interpolation
//Equation ref see: https://ccrma.stanford.edu/~jos/sasp/Quadratic_Interpolation_Spectral_Peaks.html
float parab_interpolation(const float *arr, uint32_t tau)
{
    //take 3 points of min point of parabola
    float x0 = arr[tau - 1];
    float x1 = arr[tau];
    float x2 = arr[tau + 1];

    return (float)tau + 0.5f * (x0 - x2) / (x0 - 2.0f * x1 + x2);
}




//puTTy Frequency Confidence Printing
void YIN_print(YIN_Result_t r, float cents, uint32_t led)
{
    char msg[128];
    int len;

    if (r.valid)
    {
        uint32_t freq_x100 = (uint32_t)(r.freq_hz * 100.0f);
        uint32_t conf_x100 = (uint32_t)(r.confidence * 100.0f);
        int32_t cents_x100 = (int32_t)(cents * 100.0f);

        len = snprintf(msg, sizeof(msg),
               "freq=%lu.%02lu Hz cents=%ld.%02ld LED=P%lu\r\n",
               freq_x100 / 100,
               freq_x100 % 100,
               cents_x100 / 100,
               labs(cents_x100 % 100),
               led);
    }
    else
    {
        len = snprintf(msg, sizeof(msg),
                       "freq=--- Hz confidence=0.00 BAD\r\n");
    }

    HAL_UART_Transmit(&huart6, (uint8_t *)msg, len, HAL_MAX_DELAY);
}

