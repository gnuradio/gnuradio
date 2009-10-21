/* -*- c++ -*- */
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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <vrt_source_32fc.h>
#include <vrt/expanded_header.h>
#include <vrt/copiers.h>
#include <vrt/types.h>
#include <vrt/if_context.h>
#include <gr_io_signature.h>
#include <missing_pkt_checker.h>
#include <iostream>
#include <gruel/inet.h>
#include <cstdio>


#define VERBOSE 1		// define to 0 or 1


class rx_32fc_handler : public vrt::rx_packet_handler
{
  int 			 	      d_noutput_items;
  std::complex<float>		     *d_out;
  int				     *d_oo;		// output index
  std::vector< std::complex<float> > &d_remainder;
  missing_pkt_checker		     &d_checker;
  all_context_t d_if_context;
  size_t d_if_context_n32_bit_words;

public:

  rx_32fc_handler(int noutput_items, std::complex<float> *out,
		  int *oo, std::vector< std::complex<float> > &remainder,
		  missing_pkt_checker &checker)
    : d_noutput_items(noutput_items), d_out(out),
      d_oo(oo), d_remainder(remainder), d_checker(checker),
      d_if_context_n32_bit_words(0) {}

  ~rx_32fc_handler();

  bool operator()(const uint32_t *payload,
		  size_t n32_bit_words,
		  const vrt::expanded_header *hdr);
  all_context_t* get_if_context(void){
    return d_if_context_n32_bit_words? &d_if_context : NULL;};
};

rx_32fc_handler::~rx_32fc_handler()
{
}

bool
rx_32fc_handler::operator()(const uint32_t *payload,
			    size_t n32_bit_words,
			    const vrt::expanded_header *hdr)
{
  if (hdr->if_data_p()){
    int nmissing = d_checker.check(hdr);
    if (VERBOSE && nmissing != 0){
      std::cerr << "S" << nmissing;
    }

    // copy as many as will fit into the output buffer.

    size_t n = std::min(n32_bit_words, (size_t)(d_noutput_items - *d_oo));
    vrt::copy_net_16sc_to_host_32fc(n, payload, &d_out[*d_oo]);
    *d_oo += n;

    // if there are any left over, copy them into remainder and tell
    // our caller we're had enough for now.

    size_t r = n32_bit_words - n;
    if (r > 0){
      assert(d_remainder.size() == 0);
      d_remainder.resize(r);
      vrt::copy_net_16sc_to_host_32fc(r, &payload[n], &d_remainder[0]);
      return false;		// Stop calling us.
    }

    return true;			// Keep calling us, we've got more room
  }
  else if (hdr->if_context_p()){
    // print the IF-Context packet
    fprintf(stderr, "\nIF-Context:\n");
    //for (size_t i = 0; i < n32_bit_words; i++)
    //  fprintf(stderr, "%04x: %08x\n", (unsigned int) i, ntohl(payload[i]));
    memcpy(&d_if_context, payload, sizeof(uint32_t)*n32_bit_words);
    d_if_context_n32_bit_words = n32_bit_words;
    return true;
  }
  else {
    // It's most likely an Extension Data or Extension Context packet
    // (that we don't know how to interpret...)
    fprintf(stderr, "\nIF-Extension:\n");
    for (size_t i = 0; i < n32_bit_words; i++)
      fprintf(stderr, "%04x: %08x\n", (unsigned int) i, ntohl(payload[i]));
    return true;
  }
}


// ------------------------------------------------------------------------

vrt_source_32fc::vrt_source_32fc(const char *name)

  : vrt_source_base(name,
		    gr_make_io_signature(1, 1, sizeof(gr_complex)))
{
}

vrt_source_32fc::~vrt_source_32fc()
{
  if (VERBOSE){
    std::cerr << "\nvrt_source_32fc: npackets = " << d_checker.npackets()
	      << " nwrong_pkt_cnt = " << d_checker.nwrong_pkt_cnt()
	      << " nmissing_pkt_est = " << d_checker.nmissing_pkt_est()
	      << std::endl;
  }
}

int
vrt_source_32fc::work(int noutput_items,
		      gr_vector_const_void_star &input_items,
		      gr_vector_void_star &output_items)
{
  gr_complex *out = (gr_complex *)output_items[0];
  int	oo = 0;

  // Handle any samples left over from the last call.
  int t = std::min(noutput_items, (int)d_remainder.size());
  if (t != 0){
    for (int i = 0; i < t; i++)
      out[i] = d_remainder[i];
    d_remainder.erase(d_remainder.begin(), d_remainder.begin()+t);
    oo = t;
  }
  if (noutput_items - oo == 0)
    return oo;
  
  // While we've got room, and there are packets, handle them
  rx_32fc_handler h(noutput_items, out, &oo, d_remainder, d_checker);
  bool ok = vrt_rx()->rx_packets(&h);

  if (!ok){
    std::cerr << "vrt_source_32fc: vrt::rx_packets() failed" << std::endl;
    return -1;	// say we're done
  }

  //we have a context packet, grab its useful information...
  //remember that things are in network byte order!
  if (h.get_if_context()){
    //extract caldiv stuff
    d_lo_freq = vrt_freq_to_double(ntohx(h.get_if_context()->caldiv.lo_freq));
    d_cal_freq = vrt_freq_to_double(ntohx(h.get_if_context()->caldiv.cal_freq));
    d_lo_locked = bool(ntohx(h.get_if_context()->caldiv.lo_locked));
    d_cal_locked = bool(ntohx(h.get_if_context()->caldiv.cal_locked));
    d_cal_enabled = bool(ntohx(h.get_if_context()->caldiv.cal_enabled));
    d_caldiv_temp = vrt_temp_to_double(ntohx(h.get_if_context()->caldiv.temp));
    d_caldiv_rev = ntohx(h.get_if_context()->caldiv.rev);
    d_caldiv_ser = ntohx(h.get_if_context()->caldiv.ser);
    d_caldiv_mod = ntohx(h.get_if_context()->caldiv.mod);
    //extract gps stuff
    d_utc_time = ntohx(h.get_if_context()->gps.formatted_gps.integer_secs);
    d_altitude = vrt_altitude_to_double(ntohx(h.get_if_context()->gps.formatted_gps.altitude));
    d_longitude = vrt_geo_angle_to_double(ntohx(h.get_if_context()->gps.formatted_gps.longitude));
    d_latitude = vrt_geo_angle_to_double(ntohx(h.get_if_context()->gps.formatted_gps.latitude));
  }

  return oo;
}
