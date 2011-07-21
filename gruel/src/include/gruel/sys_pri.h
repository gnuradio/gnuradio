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

#ifndef INCLUDED_GRUEL_SYS_PRI_H
#define INCLUDED_GRUEL_SYS_PRI_H

#include <gruel/api.h>
#include <gruel/realtime.h>

/*
 * A single place to define real-time priorities used by the system itself
 */
namespace gruel {

  struct GRUEL_API sys_pri {
    static rt_sched_param python();		  // python code
    static rt_sched_param normal();		  // normal blocks
    static rt_sched_param gcell_event_handler();
    static rt_sched_param usrp2_backend();	  // thread that services the ethernet
  };

}

#endif /* INCLUDED_GRUEL_SYS_PRI_H */
