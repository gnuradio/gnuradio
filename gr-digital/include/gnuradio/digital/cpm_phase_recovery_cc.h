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

#ifndef INCLUDED_DIGITAL_CPM_PHASE_RECOVERY_CC_H
#define INCLUDED_DIGITAL_CPM_PHASE_RECOVERY_CC_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Derotate a CPM (MSK, GMSK, etc.) signal.
     * \ingroup synchronizers_blk
     *
     */
    class DIGITAL_API cpm_phase_recovery_cc : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<cpm_phase_recovery_cc> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of digital::cpm_phase_recovery_cc.
       *
       * \param p: The modulation index of the CPM modulation.
       * For MSK, GMSK, etc., use 2.
       * \param len: The number of symbols to average at a time.
       * \param sps: Samples per symbol.
       */
      static sptr make(int p, int len, int sps);
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_CPM_PHASE_RECOVERY_CC_H */

