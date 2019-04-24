/*
 * MIT License
 *
 * Copyright(c) 2011-2019 The Maintainers of Nanvix
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * Copyright (C) 2013-2014 Kalray SA.
 *
 * All rights reserved.
 */

INCLUDE "vboard_u.ld"

/*
 * DDR
 */
DDR_START = DEFINED(DDR_START) ? DDR_START : 0x80000000;
DDR_SIZE = DEFINED(DDR_SIZE) ? DDR_SIZE : 0x80000000;

/*
 * Virtual Memory
 */
VIRT_U_MEM_START = ALIGN((INTERNAL_RAM_BASE + MOS_RESERVED), 0x1000);
VIRT_U_MEM_END = INTERNAL_RAM_BASE + ABSOLUTE(INTERNAL_RAM_SIZE);

/*
 * User Stack
 */
USER_STACK_SIZE = DEFINED(USER_STACK_SIZE) ? USER_STACK_SIZE : 0x1800;
TOTAL_USER_STACK_SPACE = USER_STACK_SIZE * 4;

/*
 * Boot Stack
 */
BOOTSTACK_SIZE = DEFINED(BOOTSTACK_SIZE) ? BOOTSTACK_SIZE   : 0x100;

/*
 * Kernel Stack
 */
KSTACK_SIZE = DEFINED(KSTACK_SIZE) ? KSTACK_SIZE : 0x400;

/*
 * Reset vector
 */
K1_EXCEPTION_ADDRESS = 0x0;

/*============================================================================*
 * Overridden Symbols                                                         *
 *============================================================================*/

_MOS_SECURITY_LEVEL = DEFINED(_MOS_SECURITY_LEVEL) ? _MOS_SECURITY_LEVEL : (DEFINED(_LIBNOC_DISABLE_FIFO_FULL_CHECK) ? 0 : 1);
_VBSP_OVERRIDE_PE_MASK = DEFINED(_VBSP_OVERRIDE_PE_MASK) ? _VBSP_OVERRIDE_PE_MASK : 0xffff;

RamBase = DEFINED(RamBase) ? RamBase : ORIGIN(ddr);
MPPA_SPAWN_DIRECTORY_SIZE = DEFINED(MPPA_SPAWN_DIRECTORY_SIZE) ? MPPA_SPAWN_DIRECTORY_SIZE : 8K;
MPPA_IMAGES_DIRECTORY_ADDRESS = DEFINED(MPPA_IMAGES_DIRECTORY_ADDRESS) ? MPPA_IMAGES_DIRECTORY_ADDRESS : RamBase;
MPPA_IMAGES_DIRECTORY_SIZE = DEFINED(MPPA_IMAGES_DIRECTORY_SIZE) ? MPPA_IMAGES_DIRECTORY_SIZE : 32M;

EXTERN(_vstart)

/* Definition of sections */
SECTIONS
{

	INCLUDE "prologue_link.ld"

  .spawn_dir   ALIGN(8): AT ( ALIGN(LOADADDR(.tbss) + SIZEOF(.tbss), 8))
  {
    MPPA_SPAWN_DIRECTORY_ADDRESS = ABSOLUTE(.);
    . += MPPA_SPAWN_DIRECTORY_SIZE;
  }  > internal_mem

   /* Reserve room for .tdata and .tbss for PE0 */
   .tls_rm0 ALIGN(8) :  AT ( ALIGN( LOADADDR(.spawn_dir) + SIZEOF(.spawn_dir), 8))

   {
    _tls_rm0_start = ABSOLUTE(.);
    _master_tls_start = ABSOLUTE(_tls_rm0_start);
    . += _tls_size;
    . = ALIGN(8);
    _tls_rm0_end = ABSOLUTE(.);
    _master_tls_end = ABSOLUTE(_tls_rm0_end);
   } =0x12345678

   /* Reserve room for .tdata and .tbss for PE1 */
   .tls_rm1 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_rm0) + SIZEOF(.tls_rm0), 8))
   {
    _tls_rm1_start = ABSOLUTE(.);
    . += _tls_size;
    . = ALIGN(8);
    _tls_rm1_end = ABSOLUTE(.);
   } =0x12345678

   /* Reserve room for .tdata and .tbss for PE2 */
   .tls_rm2 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_rm1) + SIZEOF(.tls_rm1), 8))
   {
    _tls_rm2_start = ABSOLUTE(.);
    . += _tls_size;
    . = ALIGN(8);
    _tls_rm2_end = ABSOLUTE(.);
   } =0x12345678

   /* Reserve room for .tdata and .tbss for PE3 */
   .tls_rm3 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_rm2) + SIZEOF(.tls_rm2), 8))
   {
    _tls_rm3_start = ABSOLUTE(.);
    . += _tls_size;
    . = ALIGN(8);
    _tls_rm3_end = ABSOLUTE(.);
   } =0x12345678

   /* Reserve room for .tdata and .tbss for PE3 */
   .tls_rm4 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_rm3) + SIZEOF(.tls_rm3), 8))
   {
    _tls_rm4_start = ABSOLUTE(.);
    . += _tls_size;
    . = ALIGN(8);
    _tls_rm4_end = ABSOLUTE(.);
   } =0x12345678

   /* Reserve room for .tdata and .tbss for PE3 */
   .tls_rm5 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_rm4) + SIZEOF(.tls_rm4), 8))
   {
    _tls_rm5_start = ABSOLUTE(.);
    . += _tls_size;
    . = ALIGN(8);
    _tls_rm5_end = ABSOLUTE(.);
   } =0x12345678

   /* Reserve room for .tdata and .tbss for PE3 */
   .tls_rm6 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_rm5) + SIZEOF(.tls_rm5), 8))
   {
    _tls_rm6_start = ABSOLUTE(.);
    . += _tls_size;
    . = ALIGN(8);
    _tls_rm6_end = ABSOLUTE(.);
   } =0x12345678

   /* Reserve room for .tdata and .tbss for PE3 */
   .tls_rm7 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_rm6) + SIZEOF(.tls_rm6), 8))
   {
    _tls_rm7_start = ABSOLUTE(.);
    . += _tls_size;
    . = ALIGN(8);
    _tls_rm7_end = ABSOLUTE(.);
   } =0x12345678

   _pag_zone_size = 0;
   _sbss_start = 0;
   _sbss_end = 0;
   _tls_rm_start = 0;

    /* Space reserved for mppa_spawn args */
  .mppa_args ALIGN(8) : AT ( ALIGN(LOADADDR(.tls_rm7) + SIZEOF(.tls_rm7), 8))
  {
    _mppa_arg_start = ABSOLUTE(.);
    PROVIDE(MPPA_ARGAREA_START = ABSOLUTE(.));
    . += MPPA_ARGAREA_SIZE;
    _mppa_arg_end = ABSOLUTE(.);
  }

   .uncached_data ALIGN(65536) : AT ( ALIGN(LOADADDR(.mppa_args) + SIZEOF(.mppa_args), 65536))
   {
     _uncached_data_start = ABSOLUTE(.);
     *(.uncached_data)
     . = ALIGN(., 65536);
     _uncached_data_end = ABSOLUTE(.);
   }

  .dtb ALIGN(0x100) : AT (ALIGN(LOADADDR(.uncached_data) + SIZEOF(.uncached_data), 0x100))
  {
	  __dtb_start = .;
	  KEEP(*(.dtb));
	  __dtb_end = ABSOLUTE(.);
  }
  ASSERT(__dtb_start < __dtb_end, ".dtb section for storing device tree unexpectedly empty.")

  . = ALIGN(.,0x10000);
  _kend = ABSOLUTE(.);

  . = ALIGN(.,8);
  _smem_heap_start = ABSOLUTE(.);

  . = INTERNAL_RAM_BASE + INTERNAL_RAM_SIZE;
  _smem_heap_end = ABSOLUTE(.);

  _bin_start_frame = LOADADDR(.boot) >> 12;
  _bin_end_frame   = (INTERNAL_RAM_SIZE >> 12);

  DDR_DATA_START = ORIGIN(ddr) + MPPA_IMAGES_DIRECTORY_SIZE;

  .ddr_data   DDR_DATA_START :  AT (DDR_DATA_START)
  {
	   KEEP(*(.ddr_data))
  }

  .heap   ALIGN(0x1000) :  AT (ALIGN(LOADADDR(.ddr_data) + SIZEOF(.ddr_data),0x1000))
  {
      _heap_start    = ABSOLUTE(.);
  }
  _start_async_copy = VIRT_U_MEM_START;
  _end_async_copy = VIRT_U_MEM_END;
  _size_async_copy = _end_async_copy - _start_async_copy;
  _heap_end      =  ORIGIN(ddr) + LENGTH(ddr);
  _ddr_frame_start    = (ORIGIN(ddr)) >> 12;
  _ddr_frame_end    = (ORIGIN(ddr) + LENGTH(ddr)) >> 12;

	INCLUDE "epilogue_link.ld"

	__dtb_debut_offset = ABSOLUTE(__dtb_start) - ABSOLUTE(VIRT_U_MEM_START);
	__dtb_size = ABSOLUTE(__dtb_end)- ABSOLUTE(__dtb_start);
}
