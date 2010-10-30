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
#include "msdd_rs_source_simple.h"
%}


GR_SWIG_BLOCK_MAGIC(msdd_rs,source_simple)

msdd_rs_source_simple_sptr
msdd_rs_make_source_simple (
	const char *src,
	unsigned short port_src
	);

class msdd_rs_source_simple : public gr_sync_block {
  protected:
    msdd_rs_source_simple(
	const char *src,
	unsigned short port_src
	);
 
  public:
    ~msdd_rs_source_c(); 
  int work (int noutput_items,
      gr_vector_const_void_star &input_items,
      gr_vector_void_star &output_items);

  bool start();
  bool stop();

  /* function starts the flow of data */
  int start_data();

  /* function stops the flow of data */
  int stop_data();

  long pull_adc_freq();
  /* Request the current ddc sample rate */
  float pull_ddc_samp_rate();
  /* Request the current ddc bandwidth */
  float pull_ddc_bw();
  /* Request the current rx freq */
  float pull_rx_freq();
  /* Request current ddc gain */
  int pull_ddc_gain();
  /* Request current RF attenuation */
  int pull_rf_atten();


  /*  int decim_rate(); */
  gr_vector_int gain_range();
  gr_vector_float freq_range();

  /* Set Functions */
  /*  bool set_decim_rate(unsigned int); */
  bool set_rx_freq(double); /* set_rx_freq(int,double); */
  /*  bool set_pga(int,double); */

  bool set_ddc_gain(double);
  /* Set desired sample rate of MSDD6000 -- Note bounds checking is 
  done by the module and it will return the value actually used in the hardware. */
  bool set_ddc_samp_rate(double);
  /* Set desired input BW of MSDD6000 -- Note bounds checking is 
  // done by the module and it will return the value actually used in the hardware. */
  bool set_ddc_bw(double);

  bool set_rf_atten(double);


  };

#if SWIGGUILE
%scheme %{
(load-extension "libguile-msdd_rs" "scm_init_gnuradio_msdd_rs_module")
%}

%goops %{
(use-modules (gnuradio gnuradio_core_runtime))
%}
#endif
