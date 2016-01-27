/* -*- c++ -*- */
/*
 * Copyright 2011,2012,2013 Free Software Foundation, Inc.
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

#include "constellation_receiver_cb_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include <gnuradio/expj.h>
#include <gnuradio/tag_checker.h>
#include <stdexcept>

namespace gr {
  namespace digital {

#define M_TWOPI (2*M_PI)
#define VERBOSE_MM     0     // Used for debugging symbol timing loop
#define VERBOSE_COSTAS 0     // Used for debugging phase and frequency tracking

    constellation_receiver_cb::sptr
    constellation_receiver_cb::make(constellation_sptr constell,
				    float loop_bw, float fmin, float fmax)
    {
      return gnuradio::get_initial_sptr
        (new constellation_receiver_cb_impl(constell, loop_bw,
					    fmin, fmax));
    }

    static int ios[] = {sizeof(char), sizeof(float), sizeof(float), sizeof(float), sizeof(gr_complex)};
    static std::vector<int> iosig(ios, ios+sizeof(ios)/sizeof(int));
    constellation_receiver_cb_impl::constellation_receiver_cb_impl(constellation_sptr constellation,
								   float loop_bw, float fmin, float fmax)
      : block("constellation_receiver_cb",
              io_signature::make(1, 1, sizeof(gr_complex)),
              io_signature::makev(1, 5, iosig)),
        blocks::control_loop(loop_bw, fmax, fmin),
        d_constellation(constellation),
        d_current_const_point(0)
    {
      if(d_constellation->dimensionality() != 1)
        throw std::runtime_error("This receiver only works with constellations of dimension 1.");

      message_port_register_in(pmt::mp("set_constellation"));
      set_msg_handler(
        pmt::mp("set_constellation"),
        boost::bind(&constellation_receiver_cb_impl::handle_set_constellation,
                    this, _1));

      message_port_register_in(pmt::mp("rotate_phase"));
      set_msg_handler(pmt::mp("rotate_phase"),
                      boost::bind(&constellation_receiver_cb_impl::handle_rotate_phase,
                                  this, _1));
    }

    constellation_receiver_cb_impl::~constellation_receiver_cb_impl()
    {
    }

    void
    constellation_receiver_cb_impl::phase_error_tracking(float phase_error)
    {
      advance_loop(phase_error);
      phase_wrap();
      frequency_limit();

#if VERBOSE_COSTAS
      printf("cl: phase_error: %f  phase: %f  freq: %f  sample: %f+j%f  constellation: %f+j%f\n",
	     phase_error, d_phase, d_freq, sample.real(), sample.imag(),
	     d_constellation->points()[d_current_const_point].real(),
	     d_constellation->points()[d_current_const_point].imag());
#endif
    }

    void
    constellation_receiver_cb_impl::set_phase_freq(float phase, float freq)
    {
      d_phase = phase;
      d_freq = freq;
    }

    void
    constellation_receiver_cb_impl::handle_set_constellation(pmt::pmt_t constellation_pmt)
    {
      if(pmt::is_any(constellation_pmt)) {
        boost::any constellation_any = pmt::any_ref(constellation_pmt);
        constellation_sptr constellation = boost::any_cast<constellation_sptr>(
          constellation_any);
        set_constellation(constellation);
      }
    }

    void
    constellation_receiver_cb_impl::handle_rotate_phase(pmt::pmt_t rotation)
    {
      if(pmt::is_real(rotation)) {
        double phase = pmt::to_double(rotation);
        d_phase += phase;
      }
    }

    void
    constellation_receiver_cb_impl::set_constellation(constellation_sptr constellation)
    {
      d_constellation = constellation;
    }

    int
    constellation_receiver_cb_impl::general_work(int noutput_items,
						 gr_vector_int &ninput_items,
						 gr_vector_const_void_star &input_items,
						 gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *)input_items[0];
      unsigned char *out = (unsigned char *)output_items[0];

      int i=0;

      float phase_error;
      unsigned int sym_value;
      gr_complex sample, nco;

      float *out_err = 0, *out_phase = 0, *out_freq = 0;
      gr_complex *out_symbol;
      if(output_items.size() == 5) {
        out_err = (float*)output_items[1];
        out_phase = (float*)output_items[2];
        out_freq = (float*)output_items[3];
        out_symbol = (gr_complex*)output_items[4];
      }

      std::vector<tag_t> tags;
      get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0)+ninput_items[0]);
      tag_checker tchecker(tags);

      while((i < noutput_items) && (i < ninput_items[0])) {

        std::vector<tag_t> tags_now;
        tchecker.get_tags(tags_now, i+nitems_read(0));
        for (unsigned int j=0; j<tags_now.size(); j++) {
          tag_t tag = tags_now[j];
          dispatch_msg(tag.key, tag.value);
        }

        sample = in[i];
        nco = gr_expj(d_phase);   // get the NCO value for derotating the current sample
        sample = nco*sample;      // get the downconverted symbol

        sym_value = d_constellation->decision_maker_pe(&sample, &phase_error);
        phase_error_tracking(phase_error);  // corrects phase and frequency offsets

        out[i] = sym_value;

        if(output_items.size() == 5) {
          out_err[i] = phase_error;
          out_phase[i] = d_phase;
          out_freq[i] = d_freq;
          out_symbol[i] = sample;
        }
        i++;

      }

      consume_each(i);
      return i;
    }

    void
    constellation_receiver_cb_impl::setup_rpc()
    {
#ifdef GR_CTRLPORT
      // Getters
      add_rpc_variable(
          rpcbasic_sptr(new rpcbasic_register_get<control_loop, float>(
	      alias(), "frequency",
	      &control_loop::get_frequency,
	      pmt::mp(0.0f), pmt::mp(2.0f), pmt::mp(0.0f),
	      "", "Frequency Est.", RPC_PRIVLVL_MIN,
              DISPTIME | DISPOPTSTRIP)));

      add_rpc_variable(
          rpcbasic_sptr(new rpcbasic_register_get<control_loop, float>(
	      alias(), "phase",
	      &control_loop::get_phase,
	      pmt::mp(0.0f), pmt::mp(2.0f), pmt::mp(0.0f),
	      "", "Phase Est.", RPC_PRIVLVL_MIN,
              DISPTIME | DISPOPTSTRIP)));

      add_rpc_variable(
          rpcbasic_sptr(new rpcbasic_register_get<control_loop, float>(
	      alias(), "loop_bw",
	      &control_loop::get_loop_bandwidth,
	      pmt::mp(0.0f), pmt::mp(2.0f), pmt::mp(0.0f),
	      "", "Loop bandwidth", RPC_PRIVLVL_MIN,
              DISPTIME | DISPOPTSTRIP)));

      // Setters
      add_rpc_variable(
          rpcbasic_sptr(new rpcbasic_register_set<control_loop, float>(
	      alias(), "loop_bw",
	      &control_loop::set_loop_bandwidth,
	      pmt::mp(0.0f), pmt::mp(1.0f), pmt::mp(0.0f),
	      "", "Loop bandwidth",
	      RPC_PRIVLVL_MIN, DISPNULL)));
#endif /* GR_CTRLPORT */
    }

  } /* namespace digital */
} /* namespace gr */
