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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_USRP2_THREAD_H
#define INCLUDED_USRP2_THREAD_H

#include <omnithread.h>
#include <usrp2_impl.h>

namespace usrp2 {

  class usrp2_thread : public omni_thread
  {
  private:
    usrp2::impl *d_u2;    
    
  public:
    usrp2_thread(usrp2::impl *u2);
    ~usrp2_thread();
    
    void start();
    
    virtual void *run_undetached(void *arg);
  };
  
} // namespace usrp2

#endif /* INCLUDED_USRP2_THREAD_H */
