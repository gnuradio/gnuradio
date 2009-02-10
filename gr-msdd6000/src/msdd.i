/* -*- c++ -*- */
/*
 * Copyright 2004,2009 Free Software Foundation, Inc.
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

%include "gnuradio.i"	// the common stuff

%{
#include "msdd_source_simple.h"
%}


GR_SWIG_BLOCK_MAGIC(msdd,source_simple)

msdd_source_simple_sptr
msdd_make_source_simple (
	const char *src,
	unsigned short port_src
	);

class msdd_source_simple : public gr_sync_block {
  protected:
    msdd_source_simple(
	const char *src,
	unsigned short port_src
	);
 
  public:
    ~msdd_source_c(); 
  int work (int noutput_items,
      gr_vector_const_void_star &input_items,
      gr_vector_void_star &output_items);

  bool start();
  bool stop();

  long adc_freq();
  int decim_rate();
  gr_vector_int gain_range();
  gr_vector_float freq_range();

  bool set_decim_rate(unsigned int);
  bool set_rx_freq(int,double);
  bool set_pga(int,double);
  

  };
