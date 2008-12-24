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

%{
#include "usrp_sink_c.h"
%}

GR_SWIG_BLOCK_MAGIC(usrp,sink_c)

class usrp_sink_c;
typedef boost::shared_ptr<usrp_sink_c> usrp_sink_c_sptr;

usrp_sink_c_sptr
usrp_make_sink_c(int which=0,
		 unsigned int interp_rate=32,
		 int nchan=1,
		 int mux=-1,
		 int fusb_block_size=0,
		 int fusb_nblocks=0,
		 const std::string fpga_filename="",
		 const std::string firmware_filename=""
		 ) throw (std::runtime_error);

class usrp_sink_c : public usrp_sink_base 
{
private:
  usrp_sink_c() throw (std::runtime_error);
};
