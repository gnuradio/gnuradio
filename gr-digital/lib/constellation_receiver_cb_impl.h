/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_CONSTELLATION_RECEIVER_CB_IMPL_H
#define	INCLUDED_DIGITAL_CONSTELLATION_RECEIVER_CB_IMPL_H

#include <gnuradio/digital/constellation_receiver_cb.h>
#include <gnuradio/attributes.h>
#include <gnuradio/gr_complex.h>

namespace gr {
  namespace digital {

    class constellation_receiver_cb_impl
      : public constellation_receiver_cb
    {
    public:
      constellation_receiver_cb_impl(constellation_sptr constell,
				     float loop_bw, float fmin, float fmax);

      ~constellation_receiver_cb_impl();

      void setup_rpc();

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);

    protected:
      void phase_error_tracking(float phase_error);

    private:
      constellation_sptr d_constellation;
      unsigned int d_current_const_point;

      //! Set the phase and the frequency.
      //! Typically used when we receive a tag with values for these.
      void set_phase_freq(float phase, float freq);

      /*!
       * Message handler port to receiver a new constellation.
       * constellation_pmt is a pmt_any; constellation objects have
       * an as_pmt function that can be used for this purpose.
       */
      void handle_set_constellation(pmt::pmt_t constellation_pmt);

      /*!
       * Message handler port to update the phase of the rotator. The
       * phase should be a real number (float or double) that is added
       * to the current phase. So we can rotate the constellation by
       * 90 degress by passing a value of pmt::from_double(M_PI/2).
       */
      void handle_rotate_phase(pmt::pmt_t rotation);

      //! Set the constellation used.
      //! Typically used when we receive a tag with a value for this.
      void set_constellation(constellation_sptr constellation);

      //! delay line length.
      static const unsigned int DLLEN = 8;

      //! delay line plus some length for overflow protection
      __GR_ATTR_ALIGNED(8) gr_complex d_dl[2*DLLEN];

      //! index to delay line
      unsigned int d_dl_idx;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CONSTELLATION_RECEIVER_CB_IMPL_H */
