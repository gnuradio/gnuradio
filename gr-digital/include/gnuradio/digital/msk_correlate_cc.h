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

#ifndef INCLUDED_DIGITAL_MSK_CORRELATE_CC_H
#define INCLUDED_DIGITAL_MSK_CORRELATE_CC_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Preamble correlator for (G)MSK-modulated burst transmissions
     * \ingroup synchronizers_blk
     *
     * \details
     * Correlate against a preamble sequence to provide timing, frequency, and
     * phase estimates for downstream blocks.
     *
     * This block outputs the original stream, tagging samples which correspond
     * to the preamble sequence. Four tags are issued:
     *
     * \li corr_est: the correlator output, useful for SNR estimation
     * \li time_est: the fractional sample timing offset (how early/late the
     * closest sample to the peak is to the estimated actual peak)
     * \li phase_est: the phase of the received preamble
     * \li freq_est: the estimated frequency offset in radians/sample
     *
     * Note that at least coarse frequency offset correction must have been
     * performed before this block. Also note that an AGC should be used to
     * normalize the amplitude of the incoming signal.
     *
     */
    class DIGITAL_API msk_correlate_cc : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<msk_correlate_cc> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of digital::msk_correlate.
       *
       * \param symbols: Vector of symbols containing the preamble sequence; e.g.,
       * [1,0,1,0,1,0,1,0]
       * \param bt: The bandwidth-time product. For GMSK this number is usually
       * between 0.3 and 0.6. For MSK, use a large value, e.g. 10.
       * \param sps: Samples per symbol
       */
      static sptr make(const std::vector<float> &symbols, float bt, float sps);

      virtual std::vector<gr_complex> symbols() const = 0;
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_MSK_CORRELATE_CC_H */

