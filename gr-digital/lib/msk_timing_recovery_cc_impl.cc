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
#include "msk_timing_recovery_cc_impl.h"
#include <gnuradio/filter/firdes.h>

namespace gr {
  namespace digital {

    msk_timing_recovery_cc::sptr
    msk_timing_recovery_cc::make(float sps, float gain, float limit, int osps=1)
    {
      return gnuradio::get_initial_sptr
        (new msk_timing_recovery_cc_impl(sps, gain, limit, osps));
    }

    /*
     * The private constructor
     */
    msk_timing_recovery_cc_impl::msk_timing_recovery_cc_impl(float sps, float gain, float limit, int osps)
      : gr::block("msk_timing_recovery_cc",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make3(1, 3, sizeof(gr_complex), sizeof(float), sizeof(float))),
      d_limit(limit),
      d_interp(new filter::mmse_fir_interpolator_cc()),
      d_dly_conj_1(0),
      d_dly_conj_2(0),
      d_dly_diff_1(0),
      d_mu(0.5),
      d_div(0),
      d_osps(osps)
    {
        set_sps(sps);
        enable_update_rate(true); //fixes tag propagation through variable rate blox
        set_gain(gain);
        if(d_osps != 1 && d_osps != 2) throw std::out_of_range("osps must be 1 or 2");
    }

    msk_timing_recovery_cc_impl::~msk_timing_recovery_cc_impl()
    {
        delete d_interp;
    }

    void msk_timing_recovery_cc_impl::set_sps(float sps) {
        d_sps = sps/2.0; //loop runs at 2x sps
        d_omega = d_sps;
        set_relative_rate(d_osps/sps);
//        set_history(d_sps);
    }

    float msk_timing_recovery_cc_impl::get_sps(void) {
        return d_sps;
    }

    void msk_timing_recovery_cc_impl::set_gain(float gain) {
        d_gain = gain;
        if(d_gain <= 0) throw std::out_of_range("Gain must be positive");
        d_gain_omega = d_gain*d_gain*0.25;
    }

    float msk_timing_recovery_cc_impl::get_gain(void) {
        return d_gain;
    }

    void msk_timing_recovery_cc_impl::set_limit(float limit) {
        d_limit = limit;
    }

    float msk_timing_recovery_cc_impl::get_limit(void) {
        return d_limit;
    }

    void
    msk_timing_recovery_cc_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        unsigned ninputs = ninput_items_required.size();
        for(unsigned i=0; i<ninputs; i++) {
            ninput_items_required[i] = (int)ceil((noutput_items*d_sps*2) + 3.0*d_sps + d_interp->ntaps());
        }
    }

    int
    msk_timing_recovery_cc_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const gr_complex *in = (const gr_complex *) input_items[0];
        gr_complex *out = (gr_complex *) output_items[0];
        float *out2, *out3;
        if(output_items.size() >= 2) out2 = (float *) output_items[1];
        if(output_items.size() >= 3) out3 = (float *) output_items[2];
        int oidx=0, iidx=0;
        int ninp=ninput_items[0] - 3.0*d_sps;
        if(ninp <= 0) {
            consume_each(0);
            return(0);
        }

        std::vector<tag_t> tags;
        get_tags_in_range(tags,
                          0,
                          nitems_read(0),
                          nitems_read(0)+ninp,
                          pmt::intern("time_est"));

        gr_complex sq,        //Squared input
                   dly_conj,  //Input delayed sps and conjugated
                   nlin_out,  //output of the nonlinearity
                   in_interp; //interpolated input
        float      err_out=0; //error output

        while(oidx < noutput_items && iidx < ninp) {
            //check to see if there's a tag to reset the timing estimate
            if(tags.size() > 0) {
                int offset = tags[0].offset - nitems_read(0);
                if((offset >= iidx) && (offset < (iidx+d_sps))) {
                    float center = (float) pmt::to_double(tags[0].value);
                    if(center != center) { //test for NaN, it happens somehow
                       tags.erase(tags.begin());
                       goto out;
                    }
                    if(std::abs(center) >= 1.0f) {
                        GR_LOG_WARN(d_logger, boost::format("work: ignoring time_est tag "
                                                            "(%.2f) outside of (-1, 1)") \
                                    % center);
                        tags.erase(tags.begin());
                        goto out;
                    }
                    d_mu = center;
                    iidx = offset;
                    //we want positive mu, so offset iidx to compensate
                    if(d_mu<0) {
                        d_mu++;
                        iidx--;
                    }
                    d_div = 0;
                    d_omega = d_sps;
                    d_dly_conj_2 = d_dly_conj_1;
                    //this keeps the block from outputting an odd number of
                    //samples and throwing off downstream blocks which depend
                    //on proper alignment -- for instance, a decimating FIR
                    //filter.
//                    if(d_div == 0 && d_osps == 2) oidx++;
                    tags.erase(tags.begin());
                }
            }

out:
            //the actual equation for the nonlinearity is as follows:
            //e(n) = in[n]^2 * in[n-sps].conj()^2
            //we then differentiate the error by subtracting the sample delayed by d_sps/2
            in_interp = d_interp->interpolate(&in[iidx], d_mu);
            sq = in_interp*in_interp;
            //conjugation is distributive.
            dly_conj = std::conj(d_dly_conj_2*d_dly_conj_2);
            nlin_out = sq*dly_conj;
            //TODO: paper argues that some improvement can be had
            //if you either operate at >2sps or use a better numeric
            //differentiation method.
            err_out = std::real(nlin_out - d_dly_diff_1);
            if(d_div % 2) { //error loop calc once per symbol
                err_out = gr::branchless_clip(err_out, 3.0);
                d_omega += d_gain_omega*err_out;
                d_omega  = d_sps + gr::branchless_clip(d_omega-d_sps, d_limit);
                d_mu    += d_gain*err_out;
            }
            //output every other d_sps by default.
            if(!(d_div % 2) || d_osps==2) {
                out[oidx] = in_interp;
                if(output_items.size() >= 2) out2[oidx] = err_out;
                if(output_items.size() >= 3) out3[oidx] = d_mu;
                oidx++;
            }
            d_div++;

            d_dly_conj_1 = in_interp;
            d_dly_conj_2 = d_dly_conj_1;
            d_dly_diff_1 = nlin_out;

            //update interpolator twice per symbol
            d_mu += d_omega;
            iidx    += (int)floor(d_mu);
            d_mu    -= floor(d_mu);
        }

        consume_each (iidx);
        return oidx;
    }

  } /* namespace digital */
} /* namespace gr */

