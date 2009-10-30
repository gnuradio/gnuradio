/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "usrp2_thread.h"
#include "usrp2_impl.h"
#include <gruel/realtime.h>
#include <gruel/sys_pri.h>
#include <iostream>

#define USRP2_THREAD_DEBUG 1

namespace usrp2 {

  usrp2_thread::usrp2_thread(usrp2::impl *u2) :
    omni_thread(NULL, PRIORITY_HIGH),
    d_u2(u2)
  {
  }
  
  usrp2_thread::~usrp2_thread()
  {
    // we don't own this, just forget it
    d_u2 = 0;
  }
  
  void
  usrp2_thread::start()
  {
    start_undetached();
  }
  
  void *
  usrp2_thread::run_undetached(void *arg)
  {
    if (gruel::enable_realtime_scheduling(gruel::sys_pri::usrp2_backend()) != gruel::RT_OK)
      std::cerr << "usrp2: failed to enable realtime scheduling" << std::endl;    

    // This is the first code to run in the new thread context.
    d_u2->run_data_thread();
    
    return 0;
  }

} // namespace usrp2

