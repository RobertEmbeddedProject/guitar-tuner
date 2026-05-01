#include <stdint.h>

//typedef creates an alias for struct
typedef struct{
    float freq_hz;
    float confidence;
    uint8_t valid;
} YIN_Result_t;

YIN_Result_t YIN_DetectPitch(const uint16_t *adc_buf, uint32_t n, float sample_rate_hz);
void PrintPitchResult(YIN_Result_t r);
    
    