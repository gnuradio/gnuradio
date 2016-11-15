/* -*- c++ -*- */
/*
 * Copyright 2005-2007,2010-2012 Free Software Foundation, Inc.
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

#include "mpsk_receiver_cc_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include <gnuradio/expj.h>
#include <stdexcept>

namespace gr {
  namespace digital {

#define M_TWOPI (2*M_PI)
#define VERBOSE_MM     0     // Used for debugging symbol timing loop
#define VERBOSE_COSTAS 0     // Used for debugging phase and frequency tracking

    mpsk_receiver_cc::sptr
    mpsk_receiver_cc::make(unsigned int M, float theta,
			   float loop_bw,
			   float fmin, float fmax,
			   float mu, float gain_mu,
			   float omega, float gain_omega, float omega_rel)
    {
      return gnuradio::get_initial_sptr
	(new mpsk_receiver_cc_impl(M, theta,
				   loop_bw,
				   fmin, fmax,
				   mu, gain_mu,
				   omega, gain_omega,
				   omega_rel));
    }

    mpsk_receiver_cc_impl::mpsk_receiver_cc_impl(unsigned int M, float theta,
						 float loop_bw,
						 float fmin, float fmax,
						 float mu, float gain_mu,
						 float omega, float gain_omega,
						 float omega_rel)
      : block("mpsk_receiver_cc",
              io_signature::make(1, 1, sizeof(gr_complex)),
              io_signature::make(1, 1, sizeof(gr_complex))),
	blocks::control_loop(loop_bw, fmax, fmin),
	d_M(M), d_theta(theta),
	d_current_const_point(0),
	d_mu(mu), d_gain_mu(gain_mu), d_gain_omega(gain_omega),
	d_omega_rel(omega_rel), d_max_omega(0), d_min_omega(0),
	d_p_2T(0), d_p_1T(0), d_p_0T(0), d_c_2T(0), d_c_1T(0), d_c_0T(0)
    {
      GR_LOG_WARN(d_logger, "The gr::digital::mpsk_receiver_cc block is deprecated.");

      d_interp = new gr::filter::mmse_fir_interpolator_cc();
      d_dl_idx = 0;

      set_omega(omega);

      if(omega <= 0.0)
	throw std::out_of_range("clock rate must be > 0");
      if(gain_mu <  0  || gain_omega < 0)
	throw std::out_of_range("Gains must be non-negative");

      assert(d_interp->ntaps() <= DLLEN);

      // zero double length delay line.
      for(unsigned int i = 0; i < 2 * DLLEN; i++)
	d_dl[i] = gr_complex(0.0,0.0);

      set_modulation_order(d_M);
    }

    mpsk_receiver_cc_impl::~mpsk_receiver_cc_impl()
    {
      delete d_interp;
    }

    void
    mpsk_receiver_cc_impl::set_modulation_order(unsigned int M)
    {
      // build the constellation vector from M
      make_constellation();

      // Select a phase detector and a decision maker for the modulation order
      switch(d_M) {
      case 2:  // optimized algorithms for BPSK
	d_phase_error_detector = &mpsk_receiver_cc_impl::phase_error_detector_bpsk; //bpsk;
	d_decision = &mpsk_receiver_cc_impl::decision_bpsk;
	break;

      case 4: // optimized algorithms for QPSK
	d_phase_error_detector = &mpsk_receiver_cc_impl::phase_error_detector_qpsk; //qpsk;
	d_decision = &mpsk_receiver_cc_impl::decision_qpsk;
	break;

      default: // generic algorithms for any M (power of 2?) but not pretty
	d_phase_error_detector = &mpsk_receiver_cc_impl::phase_error_detector_generic;
	d_decision = &mpsk_receiver_cc_impl::decision_generic;
	break;
      }
    }

    void
    mpsk_receiver_cc_impl::set_gain_omega_rel(float omega_rel)
    {
      d_omega_rel = omega_rel;
      set_omega(d_omega);
    }

    void
    mpsk_receiver_cc_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
      unsigned ninputs = ninput_items_required.size();
      for(unsigned i=0; i < ninputs; i++)
	ninput_items_required[i] = (int) ceil((noutput_items * d_omega) + d_interp->ntaps());
    }

    // FIXME add these back in an test difference in performance
    float
    mpsk_receiver_cc_impl::phase_error_detector_qpsk(gr_complex sample) const
    {
      float phase_error = 0;
      if(fabsf(sample.real()) > fabsf(sample.imag())) {
	if(sample.real() > 0)
	  phase_error = -sample.imag();
	else
	  phase_error = sample.imag();
      }
      else {
	if(sample.imag() > 0)
	  phase_error = sample.real();
	else
	  phase_error = -sample.real();
      }

      return phase_error;
    }

    float
    mpsk_receiver_cc_impl::phase_error_detector_bpsk(gr_complex sample) const
    {
      return -(sample.real()*sample.imag());
    }

    float mpsk_receiver_cc_impl::phase_error_detector_generic(gr_complex sample) const
    {
      //return gr::fast_atan2f(sample*conj(d_constellation[d_current_const_point]));
      return -arg(sample*conj(d_constellation[d_current_const_point]));
    }

    unsigned int
    mpsk_receiver_cc_impl::decision_bpsk(gr_complex sample) const
    {
      return (gr::branchless_binary_slicer(sample.real()) ^ 1);
      //return gr_binary_slicer(sample.real()) ^ 1;
    }

    unsigned int
    mpsk_receiver_cc_impl::decision_qpsk(gr_complex sample) const
    {
      unsigned int index;

      //index = gr::branchless_quad_0deg_slicer(sample);
      index = gr::quad_0deg_slicer(sample);
      return index;
    }

    unsigned int
    mpsk_receiver_cc_impl::decision_generic(gr_complex sample) const
    {
      unsigned int min_m = 0;
      float min_s = 65535;

      // Develop all possible constellation points and find the one that minimizes
      // the Euclidean distance (error) with the sample
      for(unsigned int m = 0; m < d_M; m++) {
	gr_complex diff = norm(d_constellation[m] - sample);

	if(fabs(diff.real()) < min_s) {
	  min_s = fabs(diff.real());
	  min_m = m;
	}
      }
      // Return the index of the constellation point that minimizes the error
      return min_m;
    }

    void
    mpsk_receiver_cc_impl::make_constellation()
    {
      for(unsigned int m = 0; m < d_M; m++) {
	d_constellation.push_back(gr_expj((M_TWOPI/d_M)*m));
      }
    }

    void
    mpsk_receiver_cc_impl::mm_sampler(const gr_complex symbol)
    {
      gr_complex sample, nco;

      d_mu--;             // skip a number of symbols between sampling
      d_phase += d_freq;  // increment the phase based on the frequency of the rotation

      // Keep phase clamped and not walk to infinity
      while(d_phase > M_TWOPI)
	d_phase -= M_TWOPI;
      while(d_phase < -M_TWOPI)
	d_phase += M_TWOPI;

      nco = gr_expj(d_phase+d_theta);   // get the NCO value for derotating the current sample
      sample = nco*symbol;      // get the downconverted symbol

      // Fill up the delay line for the interpolator
      d_dl[d_dl_idx] = sample;
      d_dl[(d_dl_idx + DLLEN)] = sample;  // put this in the second half of the buffer for overflows
      d_dl_idx = (d_dl_idx+1) % DLLEN;    // Keep the delay line index in bounds
    }

    void
    mpsk_receiver_cc_impl::mm_error_tracking(gr_complex sample)
    {
      gr_complex u, x, y;
      float mm_error = 0;

      // Make sample timing corrections

      // set the delayed samples
      d_p_2T = d_p_1T;
      d_p_1T = d_p_0T;
      d_p_0T = sample;
      d_c_2T = d_c_1T;
      d_c_1T = d_c_0T;

      d_current_const_point = (*this.*d_decision)(d_p_0T);  // make a decision on the sample value
      d_c_0T = d_constellation[d_current_const_point];

      x = (d_c_0T - d_c_2T) * conj(d_p_1T);
      y = (d_p_0T - d_p_2T) * conj(d_c_1T);
      u = y - x;
      mm_error = u.real();   // the error signal is in the real part
      mm_error = gr::branchless_clip(mm_error, 1.0); // limit mm_val

      d_omega = d_omega + d_gain_omega * mm_error;  // update omega based on loop error
      d_omega = d_omega_mid + gr::branchless_clip(d_omega-d_omega_mid, d_omega_rel);   // make sure we don't walk away

      d_mu += d_omega + d_gain_mu * mm_error;   // update mu based on loop error

#if VERBOSE_MM
      printf("mm: mu: %f   omega: %f  mm_error: %f  sample: %f+j%f  constellation: %f+j%f\n",
	     d_mu, d_omega, mm_error, sample.real(), sample.imag(),
	     d_constellation[d_current_const_point].real(), d_constellation[d_current_const_point].imag());
#endif
    }


    void
    mpsk_receiver_cc_impl::phase_error_tracking(gr_complex sample)
    {
      float phase_error = 0;

      // Make phase and frequency corrections based on sampled value
      phase_error = (*this.*d_phase_error_detector)(sample);

      advance_loop(phase_error);
      phase_wrap();
      frequency_limit();

#if VERBOSE_COSTAS
      printf("cl: phase_error: %f  phase: %f  freq: %f  sample: %f+j%f  constellation: %f+j%f\n",
	     phase_error, d_phase, d_freq, sample.real(), sample.imag(),
	     d_constellation[d_current_const_point].real(), d_constellation[d_current_const_point].imag());
#endif
}

    int
    mpsk_receiver_cc_impl::general_work(int noutput_items,
					gr_vector_int &ninput_items,
					gr_vector_const_void_star &input_items,
					gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex*)input_items[0];
      gr_complex *out = (gr_complex*)output_items[0];

      int i=0, o=0;

      while((o < noutput_items) && (i < ninput_items[0])) {
	while((d_mu > 1) && (i < ninput_items[0]))  {
	  mm_sampler(in[i]);   // puts symbols into a buffer and adjusts d_mu
	  i++;
	}

	if(i < ninput_items[0]) {
	  gr_complex interp_sample = d_interp->interpolate(&d_dl[d_dl_idx], d_mu);

	  mm_error_tracking(interp_sample);     // corrects M&M sample time
	  phase_error_tracking(interp_sample);  // corrects phase and frequency offsets

	  out[o++] = interp_sample;
	}
      }

#if 0
      printf("ninput_items: %d   noutput_items: %d   consuming: %d   returning: %d\n",
	     ninput_items[0], noutput_items, i, o);
#endif

      consume_each(i);
      return o;
    }

  } /* namespace digital */
} /* namespace gr */
