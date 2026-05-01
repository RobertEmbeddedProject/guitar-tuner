# STM32 Guitar Tuner

This project uses Multiple LEDs as the visual representation of the tuning of a guitar analog signal (A0).

## Hardware
- Board: NUCLEO-F767ZI

## Theory Notes:
Low E  ≈ 82.41 Hz
A      ≈ 110.00 Hz
D      ≈ 146.83 Hz
G      ≈ 196.00 Hz
B      ≈ 246.94 Hz
High E ≈ 329.63 Hz

#### ADC DMA buffer
remove DC offset
    Centered around 1.65, move all data back to cetner around 0
light low-pass / band-pass
amplitude gate
YIN algorithm
period estimate
frequency
nearest note comparison

### YIN Algorithm:
#define DMA_HALF_SAMPLES 2048
    Half Samples is 2048
#define DMA_TOTAL_SAMPLES (2 * DMA_HALF_SAMPLES)
    Total Samples is therefore 4096
uint16_t adc_dma_buf[DMA_TOTAL_SAMPLES];
    This is both parts of the DMA buffer, with 4096 indices
    The DMA Buffer holds 4096 samples at a time
        (half buffer and full buffer, for circular DMA)

### Peripheral Facts
216 MHz = Max STM32F767ZI core clock
PCLK1 = 16 MHz (feeds peripherals, TIM2, TIM3, etc)
TIM2 clock = 16 MHz
ADC trigger/sample rate = 20 kHz

2048 is chosen as the evaluable size of data, because at 20kHz sampling:
    Low E (longest wave) = 82 Hz
    Period = 20000 / 82 = 243 samples
    2048/243 = 8.4 cycles of the longest waveform
            This is more than enough to evaluate pitch

2 bytes a sample, so 8192 bytes of RAM is used per buffer
There are two "slots" in this circular data buffer, so half and full
    are the two slots to indicate the First In and the First Out

One sample every 1 / 20000 = 50 us
2048 samples = 2048 / 20000 = 0.1024 s
4096 samples = 4096 / 20000 = 0.2048 s
TIM2 TRGO → ADC starts one conversion

16 MHz clock
-TIM2 divider creates 20 kHz update event
-ADC conversion triggered every 50 us
-DMA copies ADC result into adc_dma_buf
-after 2048 samples, half-complete callback fires
-after 4096 samples, complete callback fires
-circular DMA wraps back to start



# Setup and SDK

## Git Cloning Advice
## Make sure ARM Compiler is installed, as well as STM32Cube Core extension
## Also install CMAKE if not already from CMAKE website
Open terminal and find ARM toolchain (usually comes with cubeIDE):
Get-ChildItem "C:\ST" -Recurse -Filter arm-none-eabi-gcc.exe -ErrorAction SilentlyContinue
Get-ChildItem "C:\Program Files" -Recurse -Filter arm-none-eabi-gcc.exe -ErrorAction SilentlyContinue
Get-ChildItem "C:\Program Files (x86)" -Recurse -Filter arm-none-eabi-gcc.exe -ErrorAction SilentlyContinue    

then run:

###              $env:Path += ";YourFilePathHereKeepSemicolon"
###              arm-none-eabi-gcc --version

## Delete Build and ReBuild
Remove-Item -Recurse -Force .\build
cmake --preset stm32-debug
cmake --build --preset stm32-debug -v


## Build and Flash
do NOT use cmake -B build -S .

First time run:
cmake --preset stm32-debug
cmake --build --preset stm32-debug

cmake --build build

## to run python:
python serial_print.py
to run python:
python serial_print.py

Press R to start recording, then R again to save a recording to /Samples

in Powershell, use this to activate serial_view.py:
python serial_view.py Samples/capture_20260426_135155.npy

# to clear command history in MSYS2 UCRT64:
history -c
# to clear command history in Powershell (shell command)
[Microsoft.PowerShell.PSConsoleReadLine]::ClearHistory()