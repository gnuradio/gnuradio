/* -*- c++ -*- */
/* 
 * Copyright 2013 Free Software Foundation, Inc.
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


#ifndef INCLUDED_DIGITAL_CORRELATE_AND_SYNC_CC_H
#define INCLUDED_DIGITAL_CORRELATE_AND_SYNC_CC_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Correlate to a preamble and send time/phase sync info
     * \ingroup synchronizers_blk
     *
     * \details
     * Input:
     * \li Stream of complex samples.
     *
     * Output:
     * \li Output stream that just passes the input complex samples
     * \li tag 'phase_est': estimate of phase offset
     * \li tag 'time_est': estimate of symbol timing offset
     * \li tag 'corr_est': the correlation value of the estimates
     * \li tag 'corr_start': the start sample of the correlation and the value
     *
     * \li Optional 2nd output stream providing the advanced correlator output
     *
     * This block is designed to search for a preamble by correlation
     * and uses the results of the correlation to get a time and phase
     * offset estimate. These estimates are passed downstream as
     * stream tags for use by follow-on synchronization blocks.
     *
     * The preamble is provided as a set of symbols along with a
     * baseband matched filter which we use to create the filtered and
     * upsampled symbol that we will receive over-the-air.
     *
     * The phase_est tag is used to adjust the phase estimation of any
     * downstream synchronization blocks and is currently used by the
     * gr::digital::costas_loop_cc block.
     *
     * The time_est tag is used to adjust the sampling timing
     * estimation of any downstream synchronization blocks and is
     * currently used by the gr::digital::pfb_clock_sync_ccf block.
     *
     * The caller must provide a "time_est" and "phase_est" tag marking
     * delay from the start of the correlated signal segment, in order
     * to mark the proper point in the preamble for downstream
     * synchronization blocks.  Generally this block cannot know
     * where the actual preamble symbols are located relative to
     * "corr_start", given that some modulations have pulses with
     * intentional ISI.  The user should manually examine the
     * primary output and the "corr_start" tag postition to
     * determine the required tag delay settings for the particular
     * modulation, preamble, and downstream blocks used.
     *
     * For a discussion of the properties of complex correlations,
     * with respect to signal processing, see:
     * Marple, Jr., S. L., "Estimating Group Delay and Phase Delay
     * via Discrete-Time 'Analytic' Cross-Correlation, _IEEE_Transcations_
     * _on_Signal_Processing_, Volume 47, No. 9, September 1999
     */
    class DIGITAL_API correlate_and_sync_cc : virtual public sync_block
    {
     public:
      typedef boost::shared_ptr<correlate_and_sync_cc> sptr;

      /*!
       * Make a block that correlates against the \p symbols vector
       * and outputs a phase and symbol timing estimate.
       *
       * \param symbols    Set of symbols to correlate against (e.g., a
       *                   preamble).
       * \param sps        Samples per symbol
       * \param mark_delay tag marking delay in samples after the
       *                   corr_start tag
       * \param threshold  Threshold of correlator, relative to a 100%
       *                   correlation (1.0). Default is 0.9.
       */
      static sptr make(const std::vector<gr_complex> &symbols,
                       float sps, unsigned int mark_delay, float threshold=0.9);

      virtual std::vector<gr_complex> symbols() const = 0;
      virtual void set_symbols(const std::vector<gr_complex> &symbols) = 0;
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_CORRELATE_AND_SYNC_CC_H */

