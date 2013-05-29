/**************************************************************/
/*  File dsk6713.cmd                                          */
/*    This linker command file can be used as the starting    */
/*  point for linking programs for the TMS320C6713 DSK.       */
/*                                                            */
/*  This CMD file assumes everything fits into internal RAM.  */
/*  If that's not true, map some sections to the external     */
/*  SDRAM.                                                    */
/**************************************************************/

-c
-heap  0x1000
-stack 0x400
-lrts6700.lib


MEMORY
{
  IRAM  : origin = 0x0,        len = 0x40000    /* 256 Kbytes */
  SDRAM : origin = 0x80000000, len = 0x1000000  /* 16 Mbytes SDRAM */
  FLASH : origin = 0x90000000, len = 0x40000    /* 256 Kbytes */
}

SECTIONS
{
  .vecs:     load = 0x00000000 /* Interrupt vectors included */
                              /* by using intr_reset()  */ 
  .text:    load = IRAM /* Executable code */ 
  .const:   load = IRAM /* Initialized constants */
  .bss:     load = IRAM /* Global and static variables */
  .data:    load = IRAM /* Data from .asm programs */
  .cinit:   load = IRAM /* Tables for initializing */
                                /* variables and constants */
  .stack:   load = IRAM /* Stack for local variables */
  .far:     load = IRAM /* Global and static variables */
                                /* declared far */
  .sysmem:  load = IRAM /* Used by malloc, etc. (heap) */
  .cio:     load = IRAM /* Used for C I/O functions */
  .csldata  load = IRAM
  .switch   load = IRAM
}  