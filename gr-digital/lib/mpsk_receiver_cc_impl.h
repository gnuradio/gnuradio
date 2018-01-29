/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2011,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_MPSK_RECEIVER_CC_IMPL_H
#define	INCLUDED_DIGITAL_MPSK_RECEIVER_CC_IMPL_H

#include <gnuradio/digital/mpsk_receiver_cc.h>
#include <gnuradio/attributes.h>
#include <gnuradio/gr_complex.h>
#include <fstream>
#include <gnuradio/filter/mmse_fir_interpolator_cc.h>

namespace gr {
  namespace digital {

    class mpsk_receiver_cc_impl : public mpsk_receiver_cc
    {
    public:
      mpsk_receiver_cc_impl(unsigned int M, float theta,
			    float loop_bw,
			    float fmin, float fmax,
			    float mu, float gain_mu,
			    float omega, float gain_omega, float omega_rel);
      ~mpsk_receiver_cc_impl();

      void forecast(int noutput_items, gr_vector_int &ninput_items_required);
      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);

      //! Returns the modulation order (M) currently set
      float modulation_order() const { return d_M; }

      //! Returns current value of theta
      float theta() const { return d_theta; }

      //! Returns current value of mu
      float mu() const { return d_mu; }

      //! Returns current value of omega
      float omega() const { return d_omega; }

      //! Returns mu gain factor
      float gain_mu() const { return d_gain_mu; }

      //! Returns omega gain factor
      float gain_omega() const { return d_gain_omega; }

      //! Returns the relative omega limit
      float gain_omega_rel() const {return d_omega_rel; }

      //! Sets the modulation order (M) currently
      void set_modulation_order(unsigned int M);

      //! Sets value of theta
      void set_theta(float theta) { d_theta = theta; }

      //! Sets value of mu
      void set_mu(float mu) { d_mu = mu; }

      //! Sets value of omega and its min and max values
      void set_omega(float omega) {
	d_omega = omega;
	d_min_omega = omega*(1.0 - d_omega_rel);
	d_max_omega = omega*(1.0 + d_omega_rel);
	d_omega_mid = 0.5*(d_min_omega+d_max_omega);
      }

      //! Sets value for mu gain factor
      void set_gain_mu(float gain_mu) { d_gain_mu = gain_mu; }

      //! Sets value for omega gain factor
      void set_gain_omega(float gain_omega) { d_gain_omega = gain_omega; }

      //! Sets the relative omega limit and resets omega min/max values
      void set_gain_omega_rel(float omega_rel);

    protected:
      void make_constellation();
      void mm_sampler(const gr_complex symbol);
      void mm_error_tracking(gr_complex sample);
      void phase_error_tracking(gr_complex sample);

      /*!
       * \brief Phase error detector for MPSK modulations.
       *
       * \param sample the I&Q sample from which to determine the phase error
       *
       * This function determines the phase error for any MPSK signal
       * by creating a set of PSK constellation points and doing a
       * brute-force search to see which point minimizes the Euclidean
       * distance. This point is then used to derotate the sample to
       * the real-axis and a atan (using the fast approximation
       * function) to determine the phase difference between the
       * incoming sample and the real constellation point
       *
       * This should be cleaned up and made more efficient.
       *
       * \returns the approximated phase error.
       */
      float phase_error_detector_generic(gr_complex sample) const;

      /*!
       * \brief Phase error detector for BPSK modulation.
       *
       * \param sample the I&Q sample from which to determine the phase error
       *
       * This function determines the phase error using a simple BPSK
       * phase error detector by multiplying the real and imaginary (the
       * error signal) components together. As the imaginary part goes to
       * 0, so does this error.
       *
       * \returns the approximated phase error.
       */
      float phase_error_detector_bpsk(gr_complex sample) const;

      /*!
       * \brief Phase error detector for QPSK modulation.
       *
       * \param sample the I&Q sample from which to determine the phase error
       *
       * This function determines the phase error using the limiter
       * approach in a standard 4th order Costas loop
       *
       * \returns the approximated phase error.
       */
      float phase_error_detector_qpsk(gr_complex sample) const;

      /*!
       * \brief Decision maker for a generic MPSK constellation.
       *
       * \param sample the baseband I&Q sample from which to make the decision
       *
       * This decision maker is a generic implementation that does a
       * brute-force search for the constellation point that minimizes
       * the error between it and the incoming signal.
       *
       * \returns the index to d_constellation that minimizes the error/
       */
      unsigned int decision_generic(gr_complex sample) const;

      /*!
       * \brief Decision maker for BPSK constellation.
       *
       * \param sample   the baseband I&Q sample from which to make the decision
       *
       * This decision maker is a simple slicer function that makes a
       * decision on the symbol based on its placement on the real
       * axis of greater than 0 or less than 0; the quadrature
       * component is always 0.
       *
       * \returns the index to d_constellation that minimizes the error/
       */
      unsigned int decision_bpsk(gr_complex sample) const;

      /*!
       * \brief Decision maker for QPSK constellation.
       *
       * \param sample the baseband I&Q sample from which to make the decision
       *
       * This decision maker is a simple slicer function that makes a
       * decision on the symbol based on its placement versus both
       * axes and returns which quadrant the symbol is in.
       *
       * \returns the index to d_constellation that minimizes the error/
       */
      unsigned int decision_qpsk(gr_complex sample) const;

    private:
      unsigned int d_M;
      float        d_theta;

      /*!
       * \brief Decision maker function pointer
       *
       * \param sample the baseband I&Q sample from which to make the decision
       *
       * This is a function pointer that is set in the constructor to
       * point to the proper decision function for the specified
       * constellation order.
       *
       * \return index into d_constellation point that is the closest to the received sample
       */
      unsigned int (mpsk_receiver_cc_impl::*d_decision)(gr_complex sample) const;

      std::vector<gr_complex> d_constellation;
      unsigned int d_current_const_point;

      // Members related to symbol timing
      float d_mu, d_gain_mu;
      float d_omega, d_gain_omega, d_omega_rel, d_max_omega, d_min_omega, d_omega_mid;
      gr_complex d_p_2T, d_p_1T, d_p_0T;
      gr_complex d_c_2T, d_c_1T, d_c_0T;

      /*!
       * \brief Phase error detector function pointer
       *
       * \param sample the I&Q sample from which to determine the phase error
       *
       * This is a function pointer that is set in the constructor to
       * point to the proper phase error detector function for the
       * specified constellation order.
       */
      float (mpsk_receiver_cc_impl::*d_phase_error_detector)(gr_complex sample) const;

      //! get interpolated value
      gr::filter::mmse_fir_interpolator_cc *d_interp;

      //! delay line length.
      static const unsigned int DLLEN = 8;

      //! delay line plus some length for overflow protection
      __GR_ATTR_ALIGNED(8) gr_complex d_dl[2*DLLEN];

      //! index to delay line
      unsigned int d_dl_idx;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_MPSK_RECEIVER_CC_IMPL_H */
