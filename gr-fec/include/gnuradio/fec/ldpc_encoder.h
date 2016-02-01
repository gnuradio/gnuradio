/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
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

#ifndef INCLUDED_LDPC_ENCODER_H
#define INCLUDED_LDPC_ENCODER_H

#include <gnuradio/fec/encoder.h>
#include <string>
#include <vector>

namespace gr {
  namespace fec {

    class FEC_API ldpc_encoder : virtual public generic_encoder
    {
    public:
      static generic_encoder::sptr make (std::string alist_file);

      virtual double rate() = 0;
      virtual bool set_frame_size(unsigned int frame_size) = 0;
      virtual int get_output_size() = 0;
      virtual int get_input_size() = 0;
    };

  }
}

#endif /* INCLUDED_LDPC_ENCODER_H */
