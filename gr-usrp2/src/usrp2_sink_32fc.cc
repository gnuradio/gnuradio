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

#include <usrp2_sink_32fc.h>
#include <usrp2/metadata.h>
#include <gr_io_signature.h>
#include <iostream>

usrp2_sink_32fc_sptr
usrp2_make_sink_32fc(const std::string &ifc, const std::string &mac_addr) 
  throw (std::runtime_error)
{
  return usrp2_sink_32fc_sptr(new usrp2_sink_32fc(ifc, mac_addr));
}

usrp2_sink_32fc::usrp2_sink_32fc(const std::string &ifc, const std::string &mac_addr) 
  throw (std::runtime_error)
  : usrp2_sink_base("usrp2_sink_32fc",
		    gr_make_io_signature(1, 1, sizeof(gr_complex)),
		    ifc, mac_addr)
{
  // NOP
}

usrp2_sink_32fc::~usrp2_sink_32fc()
{
  // NOP
}

int
usrp2_sink_32fc::work(int noutput_items,
		      gr_vector_const_void_star &input_items,
		      gr_vector_void_star &output_items)
{
  gr_complex *in = (gr_complex *)input_items[0];

  usrp2::tx_metadata metadata;
  metadata.timestamp = -1;
  metadata.send_now = 1;
  metadata.start_of_burst = 1;

  bool ok = d_u2->tx_32fc(0, // FIXME: use channel number instead of 0
			  in, noutput_items, &metadata);
  if (!ok){
    std::cerr << "usrp2_sink_32fc: tx_32fc failed" << std::endl;
    return -1;	// say we're done
  }

  return noutput_items;
}
