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

#ifndef INCLUDED_LDPC_PAR_MTRX_ENCODER_H
#define INCLUDED_LDPC_PAR_MTRX_ENCODER_H

#include <gnuradio/fec/encoder.h>
#include <gnuradio/fec/ldpc_H_matrix.h>
#include <gnuradio/fec/fec_mtrx.h>
#include <string>
#include <vector>

namespace gr {
  namespace fec {
    namespace code {

      class FEC_API ldpc_par_mtrx_encoder : virtual public generic_encoder
      {
      public:
        static generic_encoder::sptr make(std::string alist_file, unsigned int gap=0);
        static generic_encoder::sptr make_H(const code::ldpc_H_matrix::sptr H_obj);

        virtual double rate() = 0;
        virtual bool set_frame_size(unsigned int frame_size) = 0;
        virtual int get_output_size() = 0;
        virtual int get_input_size() = 0;
      };

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_LDPC_PAR_MTRX_ENCODER_H */
