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

#ifndef INCLUDED_LDPC_ENCODER_IMPL_H
#define INCLUDED_LDPC_ENCODER_IMPL_H

#include <gnuradio/fec/ldpc_encoder.h>
#include <gnuradio/fec/cldpc.h>
#include <gnuradio/fec/alist.h>
#include <map>
#include <string>
#include <vector>

namespace gr {
  namespace fec {

    class ldpc_encoder_impl : public ldpc_encoder
    {
    private:
      //plug into the generic fec api
      void generic_work(void *inBuffer, void *outbuffer);

      // memory allocated for processing
      int outputSize;
      int inputSize;
      alist d_list;
      cldpc d_code;

    public:
      ldpc_encoder_impl(std::string alist_file);
      ~ldpc_encoder_impl();

      double rate() { return (1.0*get_input_size() / get_output_size()); }
      bool set_frame_size(unsigned int frame_size) { return false; }
      int get_output_size();
      int get_input_size();
    };

  }
}

#endif /* INCLUDED_LDPC_ENCODER_IMPL_H */
