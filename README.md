# STM32 Guitar Tuner

This project uses Multiple LEDs as the visual representation of the tuning of a guitar analog signal (A0).

## Hardware
- Board: NUCLEO-F767ZI

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


##Software Development Notes
At first, I set up the GPIO and board testing using CubeMX as a boilerplate code generator. I stripped out the boiler plate, and after migrating the project to be CMake and GCC built within VScode, I committed to only using CubeMX as a separate .ioc template generator for copy+pasting when I made hardware changes.

Within the CubeMX program, I can see most of the HAL compatible hardware changes between: stm32f7xx_hal_msp.c (MCU Support Package, handles mapping and peripheral specific init) main.c (peripheral initialization and implementation)

The first branch tested blinked a tuner LED and transmitted a heartbeat word to a PuTTy terminal over UART to USB. I utilized the HAL libraries and STM32 document RM0410 to fill in the blanks of any peripheral setup I wasn't sure about.

After this all functioned correctly, I added the analog input to A0 and found a separate, isolated python script online to graph the incoming data. Even at a baud rate of 460800, the data was unusably slow. At this point, I was single-looping and polling both UART as well as the ADC signal, which is both slow and inefficient usage of CPU processing.

The next best improvements will likely be, in this order: -DMA buffered analog data to speed up data aquisition for more data points and reduce CPU waste -circular or normal? -DMA buffered UART transmission -Possibly an RTOS, not really needed for this basic of a program, but the processor has plenty of room to accomodate it and it would give me a lot more control via task handling and schedule.

###DMA for ADC1 (guitar signal) Circular instead of Normal: repeated sampling into a rolling buffer Peripheral increment disabled: the ADC data register is always the same address, so I do not want it incrementing Memory increment enabled: each new sample should go to the next element in the buffer Half Word / Half Word: ADC samples are naturally 16-bit in HAL buffers, even if the converter is 12-bit FIFO disabled: Not neccessary, as far as I have read, this is added complexity for minimal gain in this application

With the circular buffer, I used half and full callbacks to be read by the CPU so that data would not get read while also being written to by the DMA buffer.