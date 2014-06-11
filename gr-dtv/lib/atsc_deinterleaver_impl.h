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

#ifndef INCLUDED_DTV_ATSC_DEINTERLEAVER_IMPL_H
#define INCLUDED_DTV_ATSC_DEINTERLEAVER_IMPL_H

#include <gnuradio/dtv/atsc_deinterleaver.h>
#include "atsc_types.h"
#include "interleaver_fifo.h"

namespace gr {
  namespace dtv {

    class atsc_deinterleaver_impl : public atsc_deinterleaver
    {
    private:
      //! transform a single symbol
      unsigned char transform(unsigned char input) {
        unsigned char retval = m_fifo[m_commutator]->stuff(input);
        m_commutator++;
        if (m_commutator >= 52)
          m_commutator = 0;
        return retval;
      }

      /*!
       * Note: The use of the alignment_fifo keeps the encoder and decoder
       * aligned if both are synced to a field boundary.  There may be other
       * ways to implement this function.  This is a best guess as to how
       * this should behave, as we have no test vectors for either the
       * interleaver or deinterleaver.
       */
      interleaver_fifo<unsigned char> alignment_fifo;

      int m_commutator;
      std::vector<interleaver_fifo<unsigned char> *> m_fifo;

    public:
      atsc_deinterleaver_impl();
      ~atsc_deinterleaver_impl();

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);

      //! reset interleaver (flushes contents and resets commutator)
      void reset();

      //! sync interleaver (resets commutator, but doesn't flush fifos)
      void sync() { m_commutator = 0; }
    };

  } /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_DTV_ATSC_DEINTERLEAVER_IMPL_H */
