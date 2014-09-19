/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include <volk/volk.h>
#include <complex>
#include "cpm_phase_recovery_cc_impl.h"

namespace gr {
  namespace digital {

    cpm_phase_recovery_cc::sptr
    cpm_phase_recovery_cc::make(int p, int len, int sps=1)
    {
      return gnuradio::get_initial_sptr
        (new cpm_phase_recovery_cc_impl(p, len, sps));
    }

    /*
     * The private constructor
     */
    cpm_phase_recovery_cc_impl::cpm_phase_recovery_cc_impl(int p, int len, int sps)
      : gr::sync_block("cpm_phase_recovery_cc",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
        d_p(p),
        d_len(len),
        d_sps(sps)
    {
        set_output_multiple(d_len*d_sps);
    }

    /*
     * Our virtual destructor.
     */
    cpm_phase_recovery_cc_impl::~cpm_phase_recovery_cc_impl()
    {
    }

    int
    cpm_phase_recovery_cc_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const gr_complex *in = (const gr_complex *) input_items[0];
        gr_complex *out = (gr_complex *) output_items[0];

        int i=0;
        gr_complex sum(0);
        gr_complex ip, ip2;
        float arg=0, avg=0;
        gr_complex rot(0);
        while(i < noutput_items) {
            //sum symbols over observation window
//            volk_32fc_s32f_power_32fc(sumvec, &in[i], 2*d_p, d_len); //TODO why is this broken oh it's bc volk sucks
            for(int j=0; j<d_len*d_sps; j+=d_sps) {
                ip = pow(in[i+j], 2*d_p);
                ip2 = pow(in[i+j], 2);
                sum += ip;
                //avg is used to resolve the 90-deg ambiguity between odd/even syms. not sure if nec. for tilted trellis.
                avg += std::real(ip2)*((j % (d_sps*2)) ? -1 : 1);
            }
            //get the angle and correction rotation, there must be a faster way
            arg = fast_atan2f(sum)/(2*d_p) - (avg<0)*M_PI/2;
            rot = exp(gr_complex(0,1)*(0-arg));
            //correct phase of output
            volk_32fc_s32fc_multiply_32fc(&out[i], &in[i], rot, d_len*d_sps);
            i += d_len*d_sps;
        }
        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace digital */
} /* namespace gr */

