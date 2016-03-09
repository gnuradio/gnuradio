/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012 Free Software Foundation, Inc.
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

/* @WARNING@ */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "@IMPL_NAME@.h"
#include <algorithm>
#include <gnuradio/io_signature.h>
#include <stdexcept>
#include <algorithm>
#include <gnuradio/gr_complex.h>

namespace gr {
  namespace analog {

    @BASE_NAME@::sptr
    @BASE_NAME@::make(double sampling_freq, gr_waveform_t waveform,
		      double frequency, double ampl, @TYPE@ offset)
    {
      return gnuradio::get_initial_sptr
	(new @IMPL_NAME@(sampling_freq, waveform, frequency, ampl, offset));
    }

    @IMPL_NAME@::@IMPL_NAME@(double sampling_freq, gr_waveform_t waveform,
			     double frequency, double ampl, @TYPE@ offset)
    : sync_block("@BASE_NAME@",
		    io_signature::make(0, 0, 0),
		    io_signature::make(1, 1, sizeof(@TYPE@))),
      d_sampling_freq(sampling_freq), d_waveform(waveform),
      d_frequency(frequency), d_ampl(ampl), d_offset(offset)
    {
      set_frequency(frequency);

      message_port_register_in(pmt::mp("freq"));
      set_msg_handler(pmt::mp("freq"), boost::bind(&@IMPL_NAME@::set_frequency_msg, this, _1));
    }

    @IMPL_NAME@::~@IMPL_NAME@()
    {
    }

    void
    @IMPL_NAME@::set_frequency_msg(pmt::pmt_t msg)
    {
      // Accepts either a number that is assumed to be the new
      // frequency or a key:value pair message where the key must be
      // "freq" and the value is the new frequency.

      if(pmt::is_number(msg)) {
        set_frequency(pmt::to_double(msg));
      }
      else if(pmt::is_pair(msg)) {
        pmt::pmt_t key = pmt::car(msg);
        pmt::pmt_t val = pmt::cdr(msg);
        if(pmt::eq(key, pmt::intern("freq"))) {
          if(pmt::is_number(val)) {
              set_frequency(pmt::to_double(val));
          }
        }
        else {
          GR_LOG_WARN(d_logger, boost::format("Set Frequency Message must have "
                                              "the key = 'freq'; got '%1%'.") \
                      % pmt::write_string(key));
        }
      }
      else {
        GR_LOG_WARN(d_logger, "Set Frequency Message must be either a number or a "
                    "key:value pair where the key is 'freq'.");
      }
    }

    int
    @IMPL_NAME@::work(int noutput_items,
		      gr_vector_const_void_star &input_items,
		      gr_vector_void_star &output_items)
    {
      @TYPE@ *optr = (@TYPE@*)output_items[0];
      @TYPE@ t;

      switch(d_waveform) {

#if @IS_COMPLEX@	// complex?

      case GR_CONST_WAVE:
	t = (gr_complex) d_ampl + d_offset;
	std::fill_n(optr, noutput_items, t);
	break;

      case GR_SIN_WAVE:
      case GR_COS_WAVE:
	d_nco.sincos(optr, noutput_items, d_ampl);
	if(d_offset == gr_complex(0,0))
	  break;

	for(int i = 0; i < noutput_items; i++) {
	  optr[i] += d_offset;
	}
	break;

	/* Implements a real square wave high from -PI to 0.
	 * The imaginary square wave leads by 90 deg.
	 */
      case GR_SQR_WAVE:
	for(int i = 0; i < noutput_items; i++) {
	  if(d_nco.get_phase() < -1*M_PI/2)
	    optr[i] = gr_complex(d_ampl, 0) + d_offset;
	  else if(d_nco.get_phase() < 0)
	    optr[i] = gr_complex(d_ampl, d_ampl) + d_offset;
	  else if(d_nco.get_phase() < M_PI/2)
	    optr[i] = gr_complex(0, d_ampl) + d_offset;
	  else
	    optr[i] = d_offset;
	  d_nco.step();
	}
	break;

	/* Implements a real triangle wave rising from -PI to 0 and
	 * falling from 0 to PI. The imaginary triangle wave leads by
	 * 90 deg.
	 */
      case GR_TRI_WAVE:
	for(int i = 0; i < noutput_items; i++) {
	  if(d_nco.get_phase() < -1*M_PI/2){
	    optr[i] = gr_complex(d_ampl*d_nco.get_phase()/M_PI + d_ampl,
		 -1*d_ampl*d_nco.get_phase()/M_PI - d_ampl/2) + d_offset;
	  }
	  else if(d_nco.get_phase() < 0) {
	    optr[i] = gr_complex(d_ampl*d_nco.get_phase()/M_PI + d_ampl,
		 d_ampl*d_nco.get_phase()/M_PI + d_ampl/2) + d_offset;
	  }
	  else if(d_nco.get_phase() < M_PI/2) {
	    optr[i] = gr_complex(-1*d_ampl*d_nco.get_phase()/M_PI + d_ampl,
		 d_ampl*d_nco.get_phase()/M_PI + d_ampl/2) + d_offset;
	  }
	  else {
	    optr[i] = gr_complex(-1*d_ampl*d_nco.get_phase()/M_PI + d_ampl,
		 -1*d_ampl*d_nco.get_phase()/M_PI + 3*d_ampl/2) + d_offset;
	  }
	  d_nco.step();
	}
	break;

	/* Implements a real saw tooth wave rising from -PI to PI.
	 * The imaginary saw tooth wave leads by 90 deg.
	 */
      case GR_SAW_WAVE:
	for(int i = 0; i < noutput_items; i++) {
	  if(d_nco.get_phase() < -1*M_PI/2) {
	    optr[i] = gr_complex(d_ampl*d_nco.get_phase()/(2*M_PI) + d_ampl/2,
		 d_ampl*d_nco.get_phase()/(2*M_PI) + 5*d_ampl/4) + d_offset;
	  }
	  else {
	    optr[i] = gr_complex(d_ampl*d_nco.get_phase()/(2*M_PI) + d_ampl/2,
		 d_ampl*d_nco.get_phase()/(2*M_PI) + d_ampl/4) + d_offset;
	  }
	  d_nco.step();
	}
	break;

#else			// nope...

      case GR_CONST_WAVE:
	t = (@TYPE@)d_ampl + d_offset;
	std::fill_n(optr, noutput_items, t);
	break;

      case GR_SIN_WAVE:
	d_nco.sin(optr, noutput_items, d_ampl);
	if(d_offset == 0)
	  break;

	for(int i = 0; i < noutput_items; i++) {
	  optr[i] += d_offset;
	}
	break;

      case GR_COS_WAVE:
	d_nco.cos(optr, noutput_items, d_ampl);
	if(d_offset == 0)
	  break;

	for(int i = 0; i < noutput_items; i++) {
	  optr[i] += d_offset;
	}
	break;

	/* The square wave is high from -PI to 0. */
      case GR_SQR_WAVE:
	t = (@TYPE@)d_ampl + d_offset;
	for(int i = 0; i < noutput_items; i++) {
	  if(d_nco.get_phase() < 0)
	    optr[i] = t;
	  else
	    optr[i] = d_offset;
	  d_nco.step();
	}
	break;

	/* The triangle wave rises from -PI to 0 and falls from 0 to PI. */
      case GR_TRI_WAVE:
	for(int i = 0; i < noutput_items; i++) {
	  double t = d_ampl*d_nco.get_phase()/M_PI;
	  if (d_nco.get_phase() < 0)
	    optr[i] = static_cast<@TYPE@>(t + d_ampl + d_offset);
	  else
	    optr[i] = static_cast<@TYPE@>(-1*t + d_ampl + d_offset);
	  d_nco.step();
	}
	break;

	/* The saw tooth wave rises from -PI to PI. */
      case GR_SAW_WAVE:
	for(int i = 0; i < noutput_items; i++) {
	  t = static_cast<@TYPE@>(d_ampl*d_nco.get_phase()/(2*M_PI)
				  + d_ampl/2 + d_offset);
	  optr[i] = t;
	  d_nco.step();
	}
	break;

#endif

      default:
	throw std::runtime_error("analog::sig_source: invalid waveform");
      }

      return noutput_items;
    }

    void
    @NAME@::set_sampling_freq(double sampling_freq)
    {
      d_sampling_freq = sampling_freq;
      d_nco.set_freq (2 * M_PI * d_frequency / d_sampling_freq);
    }

    void
    @NAME@::set_waveform(gr_waveform_t waveform)
    {
      d_waveform = waveform;
    }

    void
    @NAME@::set_frequency(double frequency)
    {
      d_frequency = frequency;
      d_nco.set_freq(2 * M_PI * d_frequency / d_sampling_freq);
    }

    void
    @NAME@::set_amplitude(double ampl)
    {
      d_ampl = ampl;
    }

    void
    @NAME@::set_offset(@TYPE@ offset)
    {
      d_offset = offset;
    }

  } /* namespace analog */
} /* namespace gr */
