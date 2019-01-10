MEMORY
{
    rom (rx) : ORIGIN = 0x08000000, LENGTH = 128K
    ram (xrw)  : ORIGIN = 0x20000000, LENGTH = 20K
}

INCLUDE stm32f103x8.ld

