/*
 * MIT License
 *
 * Copyright(c) 2011-2020 The Maintainers of Nanvix
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
DDR_SIZE  = DEFINED(DDR_SIZE) ? DDR_SIZE : 0x80000000;

/*
 * Virtual Memory
 */
VIRT_U_MEM_START = ALIGN((INTERNAL_RAM_BASE + MOS_RESERVED), 0x1000);
VIRT_U_MEM_END   = INTERNAL_RAM_BASE + ABSOLUTE(INTERNAL_RAM_SIZE);

/*
 * User Stack
 */
USER_STACK_SIZE = DEFINED(USER_STACK_SIZE) ? USER_STACK_SIZE : 0x800;
TOTAL_USER_STACK_SPACE = USER_STACK_SIZE * 16;

/*
 * Boot Stack
 */
BOOTSTACK_SIZE  = DEFINED(BOOTSTACK_SIZE) ? BOOTSTACK_SIZE   : 0x100;

/*
 * Kernel Stack
 */
KSTACK_SIZE    = DEFINED(KSTACK_SIZE) ? KSTACK_SIZE : 0x400;

/*
 * Reset vector
 */
K1_EXCEPTION_ADDRESS = 0x0;

/*============================================================================*
 * Overridden Symbols                                                         *
 *============================================================================*/

MPPA_SPAWN_DIRECTORY_SIZE = DEFINED(MPPA_SPAWN_DIRECTORY_SIZE) ? MPPA_SPAWN_DIRECTORY_SIZE : 8K;
_MOS_SECURITY_LEVEL = DEFINED(_MOS_SECURITY_LEVEL) ? _MOS_SECURITY_LEVEL : (DEFINED(_LIBNOC_DISABLE_FIFO_FULL_CHECK) ? 0 : 1);
_VBSP_OVERRIDE_PE_MASK = DEFINED(_VBSP_OVERRIDE_PE_MASK) ? _VBSP_OVERRIDE_PE_MASK : 0xffff;
__MPPA_BURN_TX = DEFINED(__MPPA_BURN_TX) ? __MPPA_BURN_TX : 7;
__MPPA_BURN_FDIR = DEFINED(__MPPA_BURN_FDIR) ? __MPPA_BURN_FDIR : 4;

EXTERN(_vstart)

/* Definition of sections */
SECTIONS
{
	INCLUDE "prologue_link.ld"

   /* Reserve room for .tdata and .tbss for PE0 */
   .tls_pe0 ALIGN(8) :  AT ( ALIGN( LOADADDR(.tbss) + SIZEOF(.tbss), 8))
   {
    _tls_pe0_start = ABSOLUTE(.);
    _master_tls_start = ABSOLUTE(_tls_pe0_start);
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe0_end = ABSOLUTE(.);
    _master_tls_end = ABSOLUTE(_tls_pe0_end);
   } =0x12345678

   /* Reserve room for .tdata and .tbss for PE1 */
   .tls_pe1 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe0) + SIZEOF(.tls_pe0), 8))
   {
    _tls_pe1_start = ABSOLUTE(.);
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe1_end = ABSOLUTE(.);
   } =0x12345678

   /* Reserve room for .tdata and .tbss for PE2 */
   .tls_pe2 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe1) + SIZEOF(.tls_pe1), 8))
   {
    _tls_pe2_start = ABSOLUTE(.);
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe2_end = ABSOLUTE(.);
   } =0x12345678

   /* Reserve room for .tdata and .tbss for PE3 */
   .tls_pe3 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe2) + SIZEOF(.tls_pe2), 8))
   {
    _tls_pe3_start = ABSOLUTE(.);
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe3_end = ABSOLUTE(.);
   } =0x12345678

   /* Reserve room for .tdata and .tbss for PE4 */
   .tls_pe4 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe3) + SIZEOF(.tls_pe3), 8))
   {
    _tls_pe4_start = ABSOLUTE(.);
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe4_end = ABSOLUTE(.);
   } =0x12345678

   /* Reserve room for .tdata and .tbss for PE5 */
   .tls_pe5 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe4) + SIZEOF(.tls_pe4), 8))
   {
    _tls_pe5_start = ABSOLUTE(.);
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe5_end = ABSOLUTE(.);
   } =0x12345678

   /* Reserve room for .tdata and .tbss for PE6 */
   .tls_pe6 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe5) + SIZEOF(.tls_pe5), 8))
   {
    _tls_pe6_start = ABSOLUTE(.);
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe6_end = ABSOLUTE(.);
   } =0x12345678

   /* Reserve room for .tdata and .tbss for PE7 */
   .tls_pe7 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe6) + SIZEOF(.tls_pe6), 8))
   {
    _tls_pe7_start = ABSOLUTE(.);
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe7_end = ABSOLUTE(.);
   } =0x12345678

   /* Reserve room for .tdata and .tbss for PE8 */
   .tls_pe8 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe7) + SIZEOF(.tls_pe7), 8))
   {
    _tls_pe8_start = ABSOLUTE(.);
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe8_end = ABSOLUTE(.);
   } =0x12345678

   /* Reserve room for .tdata and .tbss for PE9 */
   .tls_pe9 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe8) + SIZEOF(.tls_pe8), 8))
   {
    _tls_pe9_start = ABSOLUTE(.);
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe9_end = ABSOLUTE(.);
   } =0x12345678

   /* Reserve room for .tdata and .tbss for PE10 */
   .tls_pe10 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe9) + SIZEOF(.tls_pe9), 8))
   {
    _tls_pe10_start = ABSOLUTE(.);
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe10_end = ABSOLUTE(.);
   } =0x12345678

   /* Reserve room for .tdata and .tbss for PE11 */
   .tls_pe11 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe10) + SIZEOF(.tls_pe10), 8))
   {
    _tls_pe11_start = ABSOLUTE(.);
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe11_end = ABSOLUTE(.);
   } =0x12345678

   /* Reserve room for .tdata and .tbss for PE12 */
   .tls_pe12 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe11) + SIZEOF(.tls_pe11), 8))
   {
    _tls_pe12_start = ABSOLUTE(.);
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe12_end = ABSOLUTE(.);
   } =0x12345678

   /* Reserve room for .tdata and .tbss for PE13 */
   .tls_pe13 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe12) + SIZEOF(.tls_pe12), 8))
   {
    _tls_pe13_start = ABSOLUTE(.);
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe13_end = ABSOLUTE(.);
   } =0x12345678

   /* Reserve room for .tdata and .tbss for PE14 */
   .tls_pe14 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe13) + SIZEOF(.tls_pe13), 8))
   {
    _tls_pe14_start = ABSOLUTE(.);
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe14_end = ABSOLUTE(.);
   } =0x12345678

   /* Reserve room for .tdata and .tbss for PE15 */
   .tls_pe15 ALIGN(8) : AT ( ALIGN( LOADADDR(.tls_pe14) + SIZEOF(.tls_pe14), 8))
   {
    _tls_pe15_start = ABSOLUTE(.);
    . += _tls_size;
    . = ALIGN(8);
    _tls_pe15_end = ABSOLUTE(.);
   } =0x12345678

   .uncached_data ALIGN(32768) : AT ( ALIGN(LOADADDR(.tls_pe15) + SIZEOF(.tls_pe15), 32768))
   {
     _uncached_data_start = ABSOLUTE(.);
     *(.uncached_data)
	 _uncached_data_stop = ABSOLUTE(.);
     . = ALIGN(., 32768);
     _uncached_data_end = ABSOLUTE(.);
   }

   _pag_zone_size = 0;
   _sbss_start = 0;
   _sbss_end = 0;
   _tls_rm_start = 0;

  _smem_heap_start = ABSOLUTE(.);
  _smem_heap_end = ABSOLUTE(.);

  . = ALIGN(.,0x8000);
  _kend = ABSOLUTE(.);

  .heap ALIGN(0x1000) :  AT ( ALIGN( LOADADDR(.uncached_data) + SIZEOF(.uncached_data), 0x1000))
  {
      _heap_start    = ABSOLUTE(.);
      . = ABSOLUTE(ROM_MEM_START);
  }
  _start_async_copy = VIRT_U_MEM_START;
  _end_async_copy = VIRT_U_MEM_END;
  _size_async_copy = _end_async_copy - _start_async_copy;
  _heap_end      =  VIRT_U_MEM_END;
  _bin_start_frame = LOADADDR(.boot) >> 12;
  _bin_end_frame   = (INTERNAL_RAM_SIZE >> 12);
  __image_start = LOADADDR(.boot);
  __image_end = _kend;

	INCLUDE "epilogue_link.ld"

}
/*
 * Reset vector
 */
K1_EXCEPTION_ADDRESS = 0x0;
