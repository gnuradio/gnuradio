/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
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

/* @WARNING@ */

#ifndef @GUARD_NAME@
#define @GUARD_NAME@

#include <digital/api.h>
#include <gr_sync_interpolator.h>

namespace gr {
  namespace digital {
    
    /*!
     * \brief Map a stream of symbol indexes (unpacked bytes or
     * shorts) to stream of float or complex constellation points in D
     * dimensions (D = 1 by default)
     * \ingroup converter_blk
     *
     * input: stream of @I_TYPE@; output: stream of @O_TYPE@
     *
     * out[n D + k] = symbol_table[in[n] D + k], k=0,1,...,D-1
     *
     * The combination of gr_packed_to_unpacked_XX followed by
     * digital_chunks_to_symbols_XY handles the general case of mapping
     * from a stream of bytes or shorts into arbitrary float
     * or complex symbols.
     *
     * \sa gr_packed_to_unpacked_bb, gr_unpacked_to_packed_bb,
     * \sa gr_packed_to_unpacked_ss, gr_unpacked_to_packed_ss,
     * \sa digital_chunks_to_symbols_bf, digital_chunks_to_symbols_bc.
     * \sa digital_chunks_to_symbols_sf, digital_chunks_to_symbols_sc.
     */

    class DIGITAL_API @NAME@ : virtual public gr_sync_interpolator
    {
    public:
      // gr::digital::@BASE_NAME@::sptr
      typedef boost::shared_ptr<@BASE_NAME@> sptr;

      /*!
       * Make a chunks-to-symbols block.
       *
       * \param symbol_table: list that maps chunks to symbols.
       * \param D: dimension of table.
       */
      static sptr make(const std::vector<@O_TYPE@> &symbol_table, const int D = 1);

      virtual int D() const = 0;
      virtual std::vector<@O_TYPE@> symbol_table() const = 0;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* @GUARD_NAME@ */
