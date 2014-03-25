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

#ifndef INCLUDED_FEC_CC_ENCODER_H
#define INCLUDED_FEC_CC_ENCODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/encoder.h>
#include <gnuradio/fec/cc_common.h>
#include <map>
#include <string>

namespace gr {
  namespace fec {
    namespace code {

      /*!
       * \brief Convolutional Code Encoding block
       * \ingroup error_coding_blk
       */
      class FEC_API cc_encoder : virtual public generic_encoder
      {
      public:
        static generic_encoder::sptr make
          (int framebits, int k,
           int rate, std::vector<int> polys,
           int start_state = 0, int end_state = 0,
           bool tailbiting = false, bool terminated = false,
           bool truncated = false, bool streaming = true);
      };

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_CC_ENCODER_H */
