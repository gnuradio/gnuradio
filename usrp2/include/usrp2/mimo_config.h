/* -*- c -*- */
/*
 * Copyright 2008,2009 Free Software Foundation, Inc.
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
#ifndef INCLUDED_USRP2_MIMO_CONFIG_H
#define INCLUDED_USRP2_MIMO_CONFIG_H

// FIXME: This duplicates the firmware usrp2_mimo_config.h file

namespace usrp2 {

  static const int _MC_WE_LOCK		= 0x0001;
  static const int _MC_MIMO_CLK_INPUT	= 0x0002;		// else SMA input
  
  /*
   * Derived masks (use these):
   *
   * We get our input from 1 of three places:
   *  Our free running oscilator, our SMA connector, or from the MIMO connector
   */
  static const int MC_WE_DONT_LOCK	= 0x0000;
  static const int MC_WE_LOCK_TO_SMA	= (_MC_WE_LOCK | 0);
  static const int MC_WE_LOCK_TO_MIMO	= (_MC_WE_LOCK | _MC_MIMO_CLK_INPUT);
  
  /*
   * Independent of the source of the clock, we may or may not drive our
   * clock onto the mimo connector.  Note that there are dedicated clock
   * signals in each direction, so disaster doesn't occurs if we're
   * unnecessarily providing clock.
   */
  static const int MC_PROVIDE_CLK_TO_MIMO = 0x0004;
}

#endif /* INCLUDED_USRP2_MIMO_CONFIG_H */
