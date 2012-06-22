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

#ifndef INCLUDED_FILTER_HILBERT_FC_H
#define INCLUDED_FILTER_HILBERT_FC_H

#include <filter/api.h>
#include <gr_sync_block.h>
#include <gr_types.h>

namespace gr {
  namespace filter {
    
    class FILTER_API hilbert_fc : virtual public gr_sync_block
    {
    public:
      // gr::filter::hilbert_fc::sptr
      typedef boost::shared_ptr<hilbert_fc> sptr;

      /*!
       * \brief Hilbert transformer.
       * \ingroup filter_blk
       *
       * real output is input appropriately delayed.
       * imaginary output is hilbert filtered (90 degree phase shift)
       * version of input.
       */
      static sptr make(unsigned int ntaps);
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_HILBERT_FC_H */
