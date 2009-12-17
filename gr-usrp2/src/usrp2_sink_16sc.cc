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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <usrp2_sink_16sc.h>
#include <gr_io_signature.h>
#include <iostream>

// FIXME hack until VRT replaces libusrp2
#define U2_MIN_SAMPLES 9

usrp2_sink_16sc_sptr
usrp2_make_sink_16sc(const std::string &ifc, const std::string &mac_addr) 
  throw (std::runtime_error)
{
  return gnuradio::get_initial_sptr(new usrp2_sink_16sc(ifc, mac_addr));
}

usrp2_sink_16sc::usrp2_sink_16sc(const std::string &ifc, const std::string &mac_addr) 
  throw (std::runtime_error)
  : usrp2_sink_base("usrp2_sink_16sc",
		    gr_make_io_signature(1, 1, sizeof(std::complex<int16_t>)),
		    ifc, mac_addr)
{
  // NOP
}

usrp2_sink_16sc::~usrp2_sink_16sc()
{
  // NOP
}

int
usrp2_sink_16sc::work(int noutput_items,
		      gr_vector_const_void_star &input_items,
		      gr_vector_void_star &output_items)
{
  std::complex<int16_t> *in = (std::complex<int16_t> *)input_items[0];

  vrt::expanded_header hdr;
  hdr.header = VRTH_PT_IF_DATA_NO_SID | VRTH_START_OF_BURST;

  bool ok = d_u2->tx_16sc(in, noutput_items, &hdr);
  if (!ok){
    std::cerr << "usrp2_sink_16sc: tx_16sc failed" << std::endl;
    return -1;	// say we're done
  }

  return noutput_items;
}
