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
     * Input: complex samples.
     * Output 0: pass through complex samples
     * tag 'phase_est': estimate of phase offset
     * tag 'timing_est': estimate of symbol timing offset
     *
     */
    class DIGITAL_API correlate_and_sync_cc : virtual public sync_block
    {
     public:
      typedef boost::shared_ptr<correlate_and_sync_cc> sptr;

      /*!
       * Make a block that correlates against the \p symbols vector
       * and outputs a phase and symbol timing estimate.
       *
       * \param symbols Set of symbols to correlate against (e.g., a
       *                preamble).
       * \param filter  Baseband matched filter (e.g., RRC)
       * \param sps     Samples per symbol
       */
      static sptr make(const std::vector<gr_complex> &symbols,
                       const std::vector<float> &filter,
                       unsigned int sps, unsigned int nfilts=32);

      virtual std::vector<gr_complex> symbols() const = 0;
      virtual void set_symbols(const std::vector<gr_complex> &symbols) = 0;
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_CORRELATE_AND_SYNC_CC_H */

