/* -*- c++ -*- */
//
// Copyright 2009 Free Software Foundation, Inc.
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

#ifndef DB_TV_RX_MIMO_H
#define DB_TV_RX_MIMO_H

#include <usrp/db_tv_rx.h>

class db_tv_rx_mimo : public db_tv_rx
{
 public:
  db_tv_rx_mimo(usrp_basic_sptr usrp, int which,double first_IF, double second_IF);
  int _refclk_divisor();
};

#endif
