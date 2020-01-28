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

  _scoreboard_offset = ABSOLUTE(_scoreboard_start) - ABSOLUTE(VIRT_U_MEM_START);
  _scb_mem_frames_array_offset = ABSOLUTE(_scb_mem_frames_array) - ABSOLUTE(_scoreboard_start);


   /* DWARF2 debugging sections */
  .debug_info       0 : { *(.debug_info) }
  .debug_line       0 : { *(.debug_line) }
  .debug_abbrev     0 : { *(.debug_abbrev) }
  .debug_frame      0 : { *(.debug_frame) }
  .debug_aranges    0 : { *(.debug_aranges) }
  .debug_pubnames   0 : { *(.debug_pubnames) }
  .debug_str        0 : { *(.debug_str) }
  .debug_funcnames  0 : { *(.debug_funcnames) }
  .debug_typenames  0 : { *(.debug_typenames) }
  .debug_varnames   0 : { *(.debug_varnames) }
  .debug_weaknames  0 : { *(.debug_weaknames) }
  .debug_macinfo    0 : { *(.debug_macinfo) }
  .debug_loc        0 : { *(.debug_loc) }

  /DISCARD/ :
  {
      *(.boot)
      *(.boot.startup)
      *(.boot.syscall)
      *(.boot.exception)
      *(.boot.interrupt)
      *(.locked.binaries_dsm)
      *(.locked.binaries_dsm_text)
      *(.locked.binaries_ddr)
  }

  /* tracepoints */
  __k1_tracepoint_data 0 :
  {
    KEEP (*(__autotraces__k1_tracepoint_data*))
    KEEP (*(__k1_tracepoint_data*))
  }

  __k1_tracepoints_locations 0 :
  {
    KEEP (*(__autotraces__k1_tracepoints_locations*))
    KEEP (*(__k1_tracepoints_locations*))
  }

  __k1_tracepoint_names 0 :
  {
    KEEP (*(__autotraces__k1_tracepoint_names*))
    KEEP (*(__k1_tracepoints_names*))
  }

  __k1_tracepoints 0 :
  {
    KEEP (*(__autotraces__k1_tracepoints))
    KEEP (*(__k1_tracepoints))
  }
