/*
*****************************************************************************
**
**  File        : stm32_flash.ld
**
**  Updated for STM32F767ZI with split RAM regions:
**    DTCMRAM : 128K @ 0x20000000
**    RAM     : 384K @ 0x20020000
**
*****************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Top of main SRAM (not DTCM) */
_estack = 0x20080000;

/* Heap and stack */
_Min_Heap_Size = 0x200;
_Min_Stack_Size = 0x400;

/* Memory regions */
MEMORY
{
  DTCMRAM (xrw) : ORIGIN = 0x20000000, LENGTH = 128K
  RAM     (xrw) : ORIGIN = 0x20020000, LENGTH = 384K
  FLASH   (rx)  : ORIGIN = 0x08000000, LENGTH = 2048K
}

/* Sections */
SECTIONS
{
  /* Startup vectors */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } >FLASH

  /* Code */
  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    *(.glue_7)
    *(.glue_7t)
    *(.eh_frame)

    KEEP(*(.init))
    KEEP(*(.fini))

    . = ALIGN(4);
    _etext = .;
  } >FLASH

  /* Constants */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } >FLASH

  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } >FLASH

  .ARM :
  {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >FLASH

  .preinit_array :
  {
    PROVIDE_HIDDEN(__preinit_array_start = .);
    KEEP(*(.preinit_array*))
    PROVIDE_HIDDEN(__preinit_array_end = .);
  } >FLASH

  .init_array :
  {
    PROVIDE_HIDDEN(__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array*))
    PROVIDE_HIDDEN(__init_array_end = .);
  } >FLASH

  .fini_array :
  {
    PROVIDE_HIDDEN(__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array*))
    PROVIDE_HIDDEN(__fini_array_end = .);
  } >FLASH

  /* Load address for initialized data */
  _sidata = LOADADDR(.data);

  /* RTT section must go in normal SRAM, not DTCM */
  .rtt (NOLOAD) :
  {
    . = ALIGN(4);
    *(.rtt)
    . = ALIGN(4);
  } >RAM

  /* Initialized data in main SRAM */
  .data :
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } >RAM AT> FLASH

  /* Uninitialized data in main SRAM */
  . = ALIGN(4);
  .bss :
  {
    _sbss = .;
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
    __bss_end__ = _ebss;
  } >RAM

  /* Heap and stack in main SRAM */
  ._user_heap_stack :
  {
    . = ALIGN(4);
    PROVIDE(end = .);
    PROVIDE(_end = .);
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(4);
  } >RAM

  /* Remove stdlib info */
  /DISCARD/ :
  {
    libc.a(*)
    libm.a(*)
    libgcc.a(*)
  }

  .ARM.attributes 0 :
  {
    *(.ARM.attributes)
  }
}