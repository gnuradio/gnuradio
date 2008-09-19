/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gruel/sys_pri.h>

namespace gruel {

  /*
   * These may need per-OS tweaking.
   *
   * Under linux virtual_pri ->	system_pri
   *   0 ->   0
   *   1 ->   5
   *   2 ->  10
   *   3 ->  15
   *   4 ->  20		// typically used by jack and pulse audio
   *   5 ->  25
   *   6 ->  30
   *   7 ->  35
   *   8 ->  40
   *   9 ->  45
   *  10 ->  50
   *  11 ->  54
   *  12 ->  59
   *  13 ->  64
   *  14 ->  69
   *  15 ->  74
   */
  rt_sched_param
  sys_pri::python()		{ return rt_sched_param(0, RT_SCHED_RR); }

  rt_sched_param
  sys_pri::normal()		{ return rt_sched_param(2, RT_SCHED_RR); }

  rt_sched_param
  sys_pri::gcell_event_handler(){ return rt_sched_param(5, RT_SCHED_FIFO); }

  rt_sched_param
  sys_pri::usrp2_backend()	{ return rt_sched_param(6, RT_SCHED_FIFO); }
}
