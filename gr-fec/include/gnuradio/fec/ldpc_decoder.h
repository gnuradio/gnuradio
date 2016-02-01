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

#ifndef INCLUDED_LDPC_DECODER_H
#define INCLUDED_LDPC_DECODER_H

typedef float INPUT_DATATYPE;
typedef unsigned char OUTPUT_DATATYPE;

#include <map>
#include <string>
#include <gnuradio/fec/decoder.h>
#include <vector>

#include <gnuradio/fec/cldpc.h>
#include <gnuradio/fec/alist.h>
#include <gnuradio/fec/awgn_bp.h>

namespace gr {
 namespace fec {


#define MAXLOG 1e7

   class FEC_API ldpc_decoder : public generic_decoder
   {
   private:
     //private constructor
     ldpc_decoder(std::string alist_file, float sigma, int max_iterations);

     //plug into the generic fec api
     int get_history();
     float get_shift();
     const char* get_conversion();
     void generic_work(void *inBuffer, void *outbuffer);
     float d_iterations;
     int d_input_size, d_output_size;
     double d_rate;

     alist d_list;
     cldpc d_code;
     awgn_bp d_spa;

   public:
     ~ldpc_decoder ();

     double rate();
     bool set_frame_size(unsigned int frame_size);

     static generic_decoder::sptr make(std::string alist_file,
                                       float sigma=0.5,
                                       int max_iterations=50);

     int get_output_size();
     int get_input_size();
     int get_input_item_size();
     int get_output_item_size();
     float get_iterations(){ return d_iterations; }
   };

 } // namespace fec
} // namespace gr

#endif /* INCLUDED_LDPC_DECODER_H */
