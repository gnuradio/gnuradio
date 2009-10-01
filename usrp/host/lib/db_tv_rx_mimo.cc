//
// Copyright 2008,2009 Free Software Foundation, Inc.
// 
// This file is part of GNU Radio
// 
// GNU Radio is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either asversion 3, or (at your option)
// any later version.
// 
// GNU Radio is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with GNU Radio; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street,
// Boston, MA 02110-1301, USA.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <usrp/db_tv_rx_mimo.h>

db_tv_rx_mimo::db_tv_rx_mimo(usrp_basic_sptr usrp, int which,
		   double first_IF, double second_IF)
  : db_tv_rx(usrp, which,first_IF,second_IF)
{
  _enable_refclk(true);//enable FPGA refclock output on gpio 0
}

int 
db_tv_rx_mimo::_refclk_divisor()
{
  return 16;// 64/16=> 4 Mhz refclock
}

