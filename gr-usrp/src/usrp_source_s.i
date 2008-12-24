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
#include "usrp_source_s.h"
%}

GR_SWIG_BLOCK_MAGIC(usrp,source_s)

class usrp_source_s;
typedef boost::shared_ptr<usrp_source_s> usrp_source_s_sptr;

usrp_source_s_sptr
usrp_make_source_s(int which=0,
		   unsigned int decim_rate=16,
		   int nchan=1,
		   int mux=-1,
		   int mode=0,
		   int fusb_block_size=0,
		   int fusb_nblocks=0,
		   const std::string fpga_filename="",
		   const std::string firmware_filename=""
		   ) throw (std::runtime_error);

class usrp_source_s : public usrp_source_base 
{
private:
  usrp_source_s() throw (std::runtime_error);
};
