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

  .mOS K1_BOOT_ADDRESS : AT ( K1_BOOT_ADDRESS )
  {
      KEEP(*(.mOS.*))
  }

  .boot VIRT_U_MEM_START :  AT (ALIGN ( LOADADDR(.mOS) + MOS_RESERVED, 0x1000))
  {
	KEEP(*(.locked.binaries))
    _bin_start = ABSOLUTE(.);
    _vstart = ABSOLUTE(.);
    *(.locked.boot)    /* boot section */
    . = ALIGN(0x8);
  }  =0x12345678

  .locked_text ALIGN(0x1000) : AT ( ALIGN( LOADADDR(.boot) + SIZEOF(.boot), 0x1000))
  {
	__text_begin = ABSOLUTE(.);
    _handler_start = ABSOLUTE(.);
    *(.locked.handler) /* trap handlers */
    *(.locked.init_code) /* trap handlers */
    *(.locked.locked_text)
    *(.locked.text)
    *(.locked.text.*)
	*(.bsp_config_text)
    _debug_start = ABSOLUTE(.);
    . += DEBUG_SIZE;
    . = ALIGN(0x8);
  }  =0x12345678
  _debug_start_lma  = LOADADDR( .locked_text ) + _debug_start - __text_begin;

	.text ALIGN(0x100) : AT ( ALIGN( LOADADDR(.locked_text) + SIZEOF(.locked_text), 0x100))
	{
		KEEP(*(.init))
		KEEP(*(.fini))
		__TEXT_START = ABSOLUTE(.);
			*(.text)
		__TEXT_END = ABSOLUTE(.);
		*(.text.*)
		*(.gnu.linkonce.t.*)
		. = ALIGN(4096);
		__text_end = ABSOLUTE(.);
	}

  .stack ALIGN(0x1000) :  AT ( ALIGN( LOADADDR(.text) + SIZEOF(.text) , 0x1000))
  {
      __user_stack_begin = ABSOLUTE(.);
      _user_stack_end     = ABSOLUTE(.);
      _master_stack_end     = ABSOLUTE(.);
      . += TOTAL_USER_STACK_SPACE;
      _master_stack_start   = ABSOLUTE(.);
      _user_stack_start   = ABSOLUTE(.);
      . = ALIGN(0x1000);
      __user_stack_end = ABSOLUTE(.);
  }

  /* This is the value of the GP base */
  .rodata ALIGN(0x1000) :  AT ( ALIGN( LOADADDR(.stack) + SIZEOF(.stack), 0x1000))  {
	__rodata_begin = ABSOLUTE(.);
    KEEP(*crtbegin.o(.ctors))
    KEEP(*crtbegin.?(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o $OTHER_EXCLUDE_FILES) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    *(.rodata.*)
    *(.rodata)
    *(.csp_caps)
    *(.locked.rodata.*)
    *(.locked.rodata)

    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o $OTHER_EXCLUDE_FILES) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    KEEP (*(.eh_frame))
    *(.jcr)
    *(.gnu.linkonce.r.*)
    *(.gcc_except_table)
    *(.gcc_except_table.*)
    . = ALIGN(0x1000);
	__rodata_end = ABSOLUTE(.);
  } =0x12345678

  .locked_data ALIGN(0x1000) : AT ( ALIGN( LOADADDR(.rodata) + SIZEOF(.rodata), 0x1000))
  {
	__data_begin = ABSOLUTE(.);
    _locked_data_start = ABSOLUTE(.);
    *(.locked.locked_data) /* trap handlers */
    *(.locked.lowmem_data) /* trap handlers */
    *(.locked.tls_start)
    *(.locked.data) /* trap handlers */
    *(.locked.data.*) /* trap handlers */
    *(.locked.bss)
    *(.locked.bss.*)
    *(.locked.ctors)
    *(.locked.dtors)
    *(.lowmem_data)
    . = ALIGN(0x8);
    __libs_ctors_start = ABSOLUTE(.);
    KEEP(*(SORT(.locked.libs_ctors.*)))
    __libs_ctors_end   = ABSOLUTE(.);
  }  =0x12345678

  .bsp_config ALIGN(0x8) : AT ( ALIGN( LOADADDR(.locked_data) + SIZEOF(.locked_data), 0x8))
  {
	  *(.locked.bsp_config_desc)
	  *(.bsp_config_desc)
	  *(.bsp_config)
  }

  MPPA_ARGAREA_SIZE  = 0x1000;
  .scoreboard ALIGN(0x100) :  AT ( ALIGN( LOADADDR(.bsp_config) + SIZEOF(.bsp_config), 0x100))
  {
      _scoreboard_start  = ABSOLUTE(.);
      KEEP(*(.locked.scoreboard))
      _scb_mem_frames_array = ABSOLUTE(.);
      . += (INTERNAL_RAM_SIZE >> 15);
      _scoreboard_end    = ABSOLUTE(.);
      _scoreboard_boot_stack_end = ABSOLUTE(.);
      . += (BOOTSTACK_SIZE*16);
      _scoreboard_boot_stack_start = ABSOLUTE(.);
      _scoreboard_kstack_end = ABSOLUTE(.);
      . += (KSTACK_SIZE*16);
      _scoreboard_kstack_start = ABSOLUTE(.);
      MPPA_ARGAREA_START = ABSOLUTE(.);
      . += MPPA_ARGAREA_SIZE;
  }

  MPPA_ARGAREA_FILE_OFFSET  =  LOADADDR( .scoreboard ) + MPPA_ARGAREA_START - _scoreboard_start ;

  /* Section used with dynamic executable */
  .interp ALIGN(0x8):   AT ( ALIGN( LOADADDR(.scoreboard) + SIZEOF(.scoreboard), 0x8))
  {
    *(.interp)
  }
  .dynstr ALIGN(0x8):   AT ( ALIGN( LOADADDR(.interp) + SIZEOF(.interp), 0x8))
  {
    *(.dynstr)
  }

  .hash ALIGN(0x8):   AT ( ALIGN( LOADADDR(.dynstr) + SIZEOF(.dynstr), 0x8))
  {
    *(.hash)
  }

  .plt ALIGN(0x8):   AT ( ALIGN (LOADADDR(.hash) + SIZEOF(.hash), 0x8))
  {
    *(.plt)
    *(.plt.*)
  }

  .got ALIGN(0x8):   AT ( ALIGN( LOADADDR(.plt) + SIZEOF(.plt), 0x8))
  {
    *(.got)
    *(.got.*)
  }

  .dynamic ALIGN(0x8):   AT ( ALIGN( LOADADDR(.got) + SIZEOF(.got), 0x8))
  {
    *(.dynamic)
  }

  .dynsym ALIGN(0x8):   AT ( ALIGN( LOADADDR(.dynamic) + SIZEOF(.dynamic), 0x8))
  {
    *(.dynsym)
  }

  .rela.dyn ALIGN(0x8):   AT ( ALIGN( LOADADDR(.dynsym) + SIZEOF(.dynsym), 0x8))
  {
    *(.rela.dyn)
  }

  .rela.plt ALIGN(0x8):   AT ( ALIGN( LOADADDR(.rela.dyn) + SIZEOF(.rela.dyn), 0x8))
  {
    *(.rela.plt)
  }

  .data   ALIGN(0x8) :  AT ( ALIGN( LOADADDR(.rela.plt) + SIZEOF(.rela.plt), 0x8))
  {
    _data_start = ABSOLUTE(.);
	__DATA_START = ABSOLUTE(.);
		*(.data)
	__DATA_END = ABSOLUTE(.);
    *(.tls_start*)
    *(.data.*)
    *(.gnu.linkonce.d.*)
    *(COMMON)
    . = ALIGN(0x20);
  }  =0x12345678


  .bss   ALIGN(0x20) :  AT ( ALIGN( LOADADDR(.data) + SIZEOF(.data), 0x20))
  {
    _bss_start = ABSOLUTE(.);
		__BSS_START = ABSOLUTE(.);
			*(.bss)
		__BSS_END = ABSOLUTE(.);
		*(.bss.*)
		*(.dynbss)
		*(.dynbss.*)
		*(.gnu.linkonce.b.*)
		. = ALIGN(0x20);
    _bss_end = ABSOLUTE(.);
  } =0x12345678

  /* TLS support */
  .tdata   ALIGN(8) : AT ( ALIGN( LOADADDR(.bss) + SIZEOF(.bss), 8 ))
  {
    _tdata_start = ABSOLUTE(.);
    *(.tdata)
    *(.tdata.*)
    . = ALIGN(8);
    _tdata_end = ABSOLUTE(.);
  } =0x12345678
  . = _tdata_end;
  _tdata_size = _tdata_end - _tdata_start;

  .tbss   ALIGN(8) : AT ( ALIGN( LOADADDR(.tdata) + SIZEOF(.tdata), 8))
  {
    _tbss_start = ABSOLUTE(.);
    *(.tbss)
    *(.tbss.*)
    *(.locked.tbss.*)
    *(.locked.tbss)
    *(.tcommon)
    *(.gnu.linkonce.tb.*)
    . = ALIGN(8);
    _tbss_end = ABSOLUTE(.);
  } =0x12345678
  . = _tbss_end;
  _tbss_size = _tbss_end - _tbss_start;

  _tls_size = _tbss_size + _tdata_size;

