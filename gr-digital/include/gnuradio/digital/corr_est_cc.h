/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_CORR_EST_CC_CC_H
#define INCLUDED_DIGITAL_CORR_EST_CC_CC_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Correlate stream with a pre-defined sequence and estimate peak
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
     * \li tag 'amp_est': 1 over the estimated amplitude
     * \li tag 'corr_start': the start sample of the correlation and the value
     *
     * \li Optional 2nd output stream providing the advanced correlator output
     *
     * This block is designed to search for a sync word by correlation
     * and uses the results of the correlation to get a time and phase
     * offset estimate. These estimates are passed downstream as
     * stream tags for use by follow-on synchronization blocks.
     *
     * The sync word is provided as a set of symbols after being
     * filtered by a baseband matched filter.
     *
     * The phase_est tag can be used by downstream blocks to adjust
     * their phase estimator/correction loops, and is currently
     * implemented by the gr::digital::costas_loop_cc block.
     *
     * The time_est tag can be used to adjust the sampling timing
     * estimate of any downstream synchronization blocks and is
     * currently implemented by the gr::digital::pfb_clock_sync_ccf
     * block.
     *
     * The caller must provide a "time_est" and "phase_est" tag
     * marking delay from the start of the correlated signal segment,
     * in order to mark the proper point in the sync word for
     * downstream synchronization blocks.  Generally this block cannot
     * know where the actual sync word symbols are located relative to
     * "corr_start", given that some modulations have pulses with
     * intentional ISI.  The user should manually examine the primary
     * output and the "corr_start" tag postition to determine the
     * required tag delay settings for the particular modulation,
     * sync word, and downstream blocks used.
     *
     * For a discussion of the properties of complex correlations,
     * with respect to signal processing, see:
     * Marple, Jr., S. L., "Estimating Group Delay and Phase Delay
     * via Discrete-Time 'Analytic' Cross-Correlation, _IEEE_Transcations_
     * _on_Signal_Processing_, Volume 47, No. 9, September 1999
     *
     */
    class DIGITAL_API corr_est_cc : virtual public sync_block
    {
    public:
      typedef boost::shared_ptr<corr_est_cc> sptr;

      enum tm_type {
        THRESHOLD_DYNAMIC,
        THRESHOLD_ABSOLUTE,
      };

      /*!
       * Make a block that correlates against the \p symbols vector
       * and outputs a phase and symbol timing estimate.
       *
       * \param symbols           Set of symbols to correlate against (e.g., a
       *                          sync word).
       * \param sps               Samples per symbol
       * \param mark_delay        tag marking delay in samples after the
       *                          corr_start tag
       * \param threshold         Threshold of correlator, relative to a 100%
       *                          correlation (1.0). Default is 0.9.
       * \param threshold_method  Method for computing threshold.
       *
       */
      static sptr make(const std::vector<gr_complex> &symbols,
                       float sps, unsigned int mark_delay, float threshold=0.9,
                       tm_type threshold_method=THRESHOLD_ABSOLUTE);

      virtual std::vector<gr_complex> symbols() const = 0;
      virtual void set_symbols(const std::vector<gr_complex> &symbols) = 0;

      virtual unsigned int mark_delay() const = 0;
      virtual void set_mark_delay(unsigned int mark_delay) = 0;

      virtual float threshold() const = 0;
      virtual void set_threshold(float threshold) = 0;
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_CORR_EST_CC_H */
