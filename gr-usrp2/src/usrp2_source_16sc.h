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

#ifndef INCLUDED_USRP2_SOURCE_16SC_H
#define INCLUDED_USRP2_SOURCE_16SC_H

#include <usrp2_source_base.h>

class usrp2_source_16sc;
typedef boost::shared_ptr<usrp2_source_16sc> usrp2_source_16sc_sptr;

usrp2_source_16sc_sptr
usrp2_make_source_16sc(const std::string &ifc="eth0",
		       const std::string &mac="")
  throw (std::runtime_error);

/*!
 * \ingroup source_blk
 * \ingroup usrp2
 */
class usrp2_source_16sc : public usrp2_source_base
{
private:
  friend usrp2_source_16sc_sptr
  usrp2_make_source_16sc(const std::string &ifc,
			 const std::string &mac) throw (std::runtime_error);
  
protected:
  usrp2_source_16sc(const std::string &ifc, const std::string &mac) throw (std::runtime_error);

public:
  ~usrp2_source_16sc();

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
};

#endif /* INCLUDED_USRP2_SOURCE_16SC_H */
