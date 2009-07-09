/* -*- c++ -*- */
//
// Copyright 2008 Free Software Foundation, Inc.
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
//

#ifndef DB_BOARDS_H
#define DB_BOARDS_H

#include <usrp/db_base.h>
#include <usrp/usrp_basic.h>

std::vector<db_base_sptr> instantiate_dbs(int dbid, usrp_basic_sptr usrp, int which_side);

#endif 


