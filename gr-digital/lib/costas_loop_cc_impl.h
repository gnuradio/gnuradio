/* -*- c++ -*- */
/*
 * Copyright 2006,2011,2012,2014 Free Software Foundation, Inc.
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


#ifndef INCLUDED_DIGITAL_COSTAS_LOOP_CC_IMPL_H
#define INCLUDED_DIGITAL_COSTAS_LOOP_CC_IMPL_H

#include <gnuradio/digital/costas_loop_cc.h>

namespace gr {
  namespace digital {

    class costas_loop_cc_impl : public costas_loop_cc
    {
    private:
      int d_order;
      float d_error;
      float d_noise;

       /*! \brief the phase detector circuit for 8th-order PSK loops.
       *
       *  \param sample complex sample
       *  \return the phase error
       */
      float phase_detector_8(gr_complex sample) const;    // for 8PSK

      /*! \brief the phase detector circuit for fourth-order loops.
       *
       *  \param sample complex sample
       *  \return the phase error
       */
      float phase_detector_4(gr_complex sample) const;    // for QPSK

      /*! \brief the phase detector circuit for second-order loops.
       *
       *  \param sample a complex sample
       *  \return the phase error
       */
      float phase_detector_2(gr_complex sample) const;    // for BPSK


      /*! \brief the phase detector circuit for 8th-order PSK
       *  loops. Uses tanh instead of slicing and the noise estimate
       *  from the message port to estimated SNR of the samples.
       *
       *  \param sample complex sample
       *  \return the phase error
       */
      float phase_detector_snr_8(gr_complex sample) const;    // for 8PSK

      /*! \brief the phase detector circuit for fourth-order
       *  loops. Uses tanh instead of slicing and the noise estimate
       *  from the message port to estimated SNR of the samples.
       *
       *  \param sample complex sample
       *  \return the phase error
       */
      float phase_detector_snr_4(gr_complex sample) const;    // for QPSK

      /*! \brief the phase detector circuit for second-order
       *  loops. Uses tanh instead of slicing and the noise estimate
       *  from the message port to estimated SNR of the samples.
       *
       *  \param sample a complex sample
       *  \return the phase error
       */
      float phase_detector_snr_2(gr_complex sample) const;    // for BPSK


      float (costas_loop_cc_impl::*d_phase_detector)(gr_complex sample) const;

    public:
      costas_loop_cc_impl(float loop_bw, int order, bool use_snr=false);
      ~costas_loop_cc_impl();

      float error() const;

      void handle_set_noise(pmt::pmt_t msg);

      void setup_rpc();

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_COSTAS_LOOP_CC_IMPL_H */
