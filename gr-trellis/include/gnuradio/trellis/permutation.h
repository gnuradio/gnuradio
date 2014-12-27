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

#ifndef INCLUDED_TRELLIS_PERMUTATION_H
#define INCLUDED_TRELLIS_PERMUTATION_H

#include <gnuradio/trellis/api.h>
#include <vector>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace trellis {

    /*!
     * \brief Permutation.
     * \ingroup trellis_coding_blk
     */
    class TRELLIS_API permutation : virtual public sync_block
    {
    public:
      // gr::trellis::permutation::sptr
      typedef boost::shared_ptr<permutation> sptr;

      static sptr make(int K, const std::vector<int> &TABLE,
		       int SYMS_PER_BLOCK, size_t NBYTES);

      virtual int K() const = 0;
      virtual std::vector<int> TABLE() const = 0;
      virtual int SYMS_PER_BLOCK() const = 0;
      virtual size_t BYTES_PER_SYMBOL() const = 0;

      virtual void set_K(int K) =0;
      virtual void set_TABLE (const std::vector<int> &table) = 0;
      virtual void set_SYMS_PER_BLOCK(int spb) =0;
      
    };

  } /* namespace trellis */
} /* namespace gr */

#endif /* INCLUDED_TRELLIS_PERMUTATION_H */
