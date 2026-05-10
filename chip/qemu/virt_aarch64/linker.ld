OUTPUT_ARCH( "aarch64" )
ENTRY( _start )

/* Convenient size constants */
__1KB__   = 0x00000400;
__4KB__   = 0x00001000;
__16KB__  = 0x00004000;
__64KB__  = 0x00010000;
__1MB__   = 0x00100000;
__2MB__   = 0x00200000;
__4MB__   = 0x00400000;
__16MB__  = 0x01000000;
__64MB__  = 0x04000000;
__128MB__ = 0x08000000;
__256MB__ = 0x10000000;
__1GB__   = 0x40000000;

/* Define the memory regions */
MEMORY
{
    /* QEMU virt AArch64 RAM starts at 0x40000000 */
    RAM (rwx) : ORIGIN = 0x40000000, LENGTH = 128M
}

__heap_size = 4M;
__stack_size = __16KB__;
__guard_size = __4KB__;

SECTIONS
{
    . = ORIGIN(RAM);
    _ram_start = .;

    .init :
    {
        _text = .;
        /* Ensure our startup.S boot code is placed first */
        KEEP(*(.text.boot)) 
        KEEP (*(SORT_NONE(.init)))
        KEEP(*(.vectors))
    } >RAM

    .text :
    {
        *(.text.startup .text.startup.*)
        *(.text .text.*)
    } >RAM

    .fini :
    {
        KEEP (*(SORT_NONE(.fini)))
        _etext = .;
    } >RAM

    .rodata.align :
    {
        . = ALIGN(8);
        _rodata = .;
    } >RAM

    .rodata :
    {
        *(.rodata .rodata.*)
        . = ALIGN(8);

        /* --- DEVICE INITIALIZATION TIERS --- */
        PROVIDE(_device_init_start = .);

        /* 1. Pre-Kernel Devices (Interrupt controllers, Timers) */
        PROVIDE(_device_PRE_KERNEL_start = .);
        KEEP(*(._device_PRE_KERNEL))
        PROVIDE(_device_PRE_KERNEL_end = .);

        /* 2. Post-Kernel Devices (UART, I2C, SPI) */
        PROVIDE(_device_POST_KERNEL_start = .);
        KEEP(*(._device_POST_KERNEL))
        PROVIDE(_device_POST_KERNEL_end = .);

        PROVIDE(_device_init_end = .);
        /* ----------------------------------- */
        
        . = ALIGN(8);
        _erodata = .;
    } >RAM

    .data.align :
    {
        . = ALIGN(8);
        _data = .;
    } >RAM

    .data :
    {
        *(.data .data.*)
        . = ALIGN(8);
        _edata = .;
    } >RAM

    .bss.align :
    {
        . = ALIGN(8);
        _bss = .;
    } >RAM

    .bss (NOLOAD) :
    {
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(8);
        _ebss = .;
    } >RAM

    _end = .;

    .heap (NOLOAD) :
    {
        . = ALIGN(8);
        _heap = .;

        /* Absolute max heap end */
        _heap_max = ORIGIN(RAM) + LENGTH(RAM) - __stack_size - __guard_size;

        /* Final heap end:
        - If __heap_size is defined → min(_heap + __heap_size, _heap_max)
        - If not defined → _heap_max
        */
        . = (DEFINED(__heap_size)
            ? MIN(_heap + __heap_size, _heap_max)
            : _heap_max);

        _eheap = .;
    } >RAM

    .guard (NOLOAD) :
    {
        . = ALIGN(8);
        _guard_start = .;
        . += __guard_size;
        _guard_end = .;
    } >RAM

    .stack (NOLOAD) :
    {
        /* AArch64 strictly requires 16-byte stack alignment */
        . = ALIGN(16);
        _sstack = .;
        . += __stack_size;
        _stack_top = .;
        _estack = _stack_top;
    } >RAM

    PROVIDE(end = .);

    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
    }

    /* --- ASSERTIONS --- */
    
    /* 1. Ensure the RAM has not overflowed (Data + BSS + Heap + Guard + Stack) */
    ASSERT(_estack <= (ORIGIN(RAM) + LENGTH(RAM)), "Error: RAM region overflowed!")

    /* 2. Ensure Heap actually fits (especially if you manually defined __heap_size) */
    ASSERT(_eheap >= _heap, "Error: Heap size is negative or invalid calculation!")

    /* 3. Ensure Stack is aligned for AArch64 (strictly requires 16-byte alignment) */
    ASSERT(_stack_top % 16 == 0, "Error: Stack top is not 16-byte aligned!")
}

/* C runtime symbols */
PROVIDE(__bss_start  = _bss);
PROVIDE(__bss_end    = _ebss);

/* Heap boundaries */
PROVIDE(__heap_start__ = _heap);
PROVIDE(__heap_end__   = _eheap);

/* Stack boundaries */
PROVIDE(__stack_start__ = _sstack);
PROVIDE(__stack_end__   = _stack_top);