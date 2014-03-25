/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_FEC_DECODER_H
#define INCLUDED_FEC_DECODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/generic_decoder.h>
#include <gnuradio/block.h>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/format.hpp>

namespace gr {
  namespace fec {

    class FEC_API decoder : virtual public block
    {
    public:
      typedef boost::shared_ptr<decoder> sptr;
      typedef boost::shared_array<unsigned char> buf_sptr;

      static sptr make(generic_decoder::sptr my_decoder,
                       size_t input_item_size,
                       size_t output_item_size);

      virtual int general_work(int noutput_items,
                               gr_vector_int& ninput_items,
                               gr_vector_const_void_star &input_items,
                               gr_vector_void_star &output_items) = 0;
      virtual int fixed_rate_ninput_to_noutput(int ninput) = 0;
      virtual int fixed_rate_noutput_to_ninput(int noutput) = 0;
      virtual void forecast(int noutput_items,
                            gr_vector_int& ninput_items_required) = 0;
    };

  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_DECODER_H */
