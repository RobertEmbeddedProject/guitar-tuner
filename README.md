# PROJECT_NAME

STM32F767ZI Guitar Tuner

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


