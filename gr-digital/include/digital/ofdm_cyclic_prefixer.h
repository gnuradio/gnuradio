/* -*- c++ -*- */
/*
 * Copyright 2004-2006,2011,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_OFDM_CYCLIC_PREFIXER_H
#define INCLUDED_DIGITAL_OFDM_CYCLIC_PREFIXER_H

#include <digital/api.h>
#include <gr_sync_interpolator.h>

namespace gr {
  namespace digital {
    
    /*!
     * \brief adds a cyclic prefix vector to an input size long ofdm
     * symbol(vector) and converts vector to a stream output_size
     * long. 
     * \ingroup ofdm_blk
     */
    class DIGITAL_API ofdm_cyclic_prefixer : virtual public gr_sync_interpolator
    {
    public:
      // gr::digital::ofdm_cyclic_prefixer::sptr
      typedef boost::shared_ptr<ofdm_cyclic_prefixer> sptr;

      /*!
       * Make an OFDM cyclic prefix adder block.
       *
       * \param input_size size of the input symbol
       * \param output_size output of the symbol
       *        (CP len = output_size - input_size)
       */
      static sptr make(size_t input_size, size_t output_size);
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_OFDM_CYCLIC_PREFIXER_H */
