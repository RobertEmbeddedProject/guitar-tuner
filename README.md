# STM32 Guitar Tuner

This project uses Multiple LEDs as the visual representation of the tuning of a guitar analog signal (A0).
The board utilizes a DMA stream for faster analog data response and turns on a single LED in an array of LEDs to illustrate tuning.

R1  R2  R3  R4  (W)  Y1  Y2  Y3  Y4  (W)  G1  G2  G3  (W)  Y5  Y6  Y7  Y8  (W)  R5  R6  R7  R8  R9
                                 ^

Above is how the LEDs are arranged. The (W) White LEDs are for status indication only, they aren't normally illuminated.
The Red and Yellow LEDs display poor tuning and how far off the selected guitar string is from Green (good) in the center.

The software assesses the waveform to decide what the intended tuning note is; slightly off from a certain frequency tells the 
program to zero in on that frequency/note/string. 


# Hardware
- Board: NUCLEO-F767ZI

See Fritzing sketch with diagram and calculations
- MCP6002 Op Amp for 2 stage signal gain
- Midpoint voltage divider for Vref = Vdd (R4/(R3+R4))
- Fc = 1 / 2pi(R6*C4) = 1.33 MHz

## Git Cloning Advice (New Host setup)
# Make sure ARM Compiler is installed, as well as STM32Cube Core extension
# Also install CMAKE if not already from CMAKE website
Open terminal and find ARM toolchain (usually comes with cubeIDE):
Get-ChildItem "C:\ST" -Recurse -Filter arm-none-eabi-gcc.exe -ErrorAction SilentlyContinue
Get-ChildItem "C:\Program Files" -Recurse -Filter arm-none-eabi-gcc.exe -ErrorAction SilentlyContinue
Get-ChildItem "C:\Program Files (x86)" -Recurse -Filter arm-none-eabi-gcc.exe -ErrorAction SilentlyContinue    

then run:
###              $env:Path += ";YourFilePathHereKeepSemicolon"
###              arm-none-eabi-gcc --version


### Delete Build and ReBuild:
Remove-Item -Recurse -Force .\build
cmake --preset stm32-debug
cmake --build --preset stm32-debug -v


## Build and Flash
Don't use cmake -B build -S .

###First time, run:
cmake --preset stm32-debug
cmake --build --preset stm32-debug

cmake --build build


# Software Development Notes

At first, I set up the GPIO and board testing using CubeMX as a boilerplate code generator.
I stripped out the boiler plate, and after migrating the project to be CMake and GCC built within VScode,
I committed to only using CubeMX as a separate .ioc template generator for copy+pasting when I made hardware changes.

Within the CubeMX program, I can see most of the HAL compatible hardware changes between:
    stm32f7xx_hal_msp.c     (MCU Support Package, handles mapping and peripheral specific init)
    main.c                  (peripheral initialization and implementation)

The first branch tested blinked a tuner LED and transmitted a heartbeat word to a PuTTy terminal over UART to USB.
I utilized the HAL libraries and STM32 document RM0410 to fill in the blanks of any peripheral setup I wasn't sure about.

After this all functioned correctly, I added the analog input to A0 and found a separate, isolated python script online to graph
the incoming data. Even at a baud rate of 460800, the data was unusably slow. At this point, I was single-looping and polling both UART as well
as the ADC signal, which is both slow and inefficient usage of CPU processing.

The next best improvements will likely be, in this order:
-DMA buffered analog data to speed up data aquisition for more data points and reduce CPU waste
    -circular or normal?
-DMA buffered UART transmission
-Possibly an RTOS, not really needed for this basic of a program, but the processor has plenty of room to accomodate it
    and it would give me a lot more control via task handling and schedule.


rgba(221, 201, 170, 0.07) DMA for ADC1 (guitar signal)
Circular instead of Normal: repeated sampling into a rolling buffer
Peripheral increment disabled: the ADC data register is always the same address, so I do not want it incrementing
Memory increment enabled: each new sample should go to the next element in the buffer
Half Word / Half Word: ADC samples are naturally 16-bit in HAL buffers, even if the converter is 12-bit
FIFO disabled: Not neccessary, as far as I have read, this is added complexity for minimal gain in this application

With the circular buffer, I used half and full callbacks to be read by the CPU so that data would not get read
while also being written to by the DMA buffer.

