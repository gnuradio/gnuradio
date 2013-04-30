/* -*- c++ -*- */
/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_VOCODER_G723_24_DECODE_BS_IMPL_H
#define INCLUDED_VOCODER_G723_24_DECODE_BS_IMPL_H

#include <gnuradio/vocoder/g723_24_decode_bs.h>

extern "C" {
#include "g7xx/g72x.h"
}

namespace gr {
  namespace vocoder {

    class g723_24_decode_bs_impl : public g723_24_decode_bs
    {
    private:
      g72x_state d_state;

    public:
      g723_24_decode_bs_impl();
      ~g723_24_decode_bs_impl();

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_G723_24_DECODE_BS_IMPL_H */
