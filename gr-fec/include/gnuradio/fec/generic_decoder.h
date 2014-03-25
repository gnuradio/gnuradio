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

#ifndef INCLUDED_FEC_GENERIC_DECODER_H
#define INCLUDED_FEC_GENERIC_DECODER_H

#include <gnuradio/fec/api.h>
#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>

namespace gr {
  namespace fec {

    class FEC_API generic_decoder
    {
    public:
      friend class decoder;
      virtual void generic_work(void *inBuffer, void *outBuffer) = 0;
      static int base_unique_id;
      int my_id;
      int unique_id();
      std::string d_name;
      std::string alias(){ return (boost::format("%s%d")%d_name%unique_id()).str(); }

    public:
      typedef boost::shared_ptr<generic_decoder> sptr;

      generic_decoder(void) {};
      generic_decoder(std::string name);
      virtual ~generic_decoder();

      virtual int get_input_size() = 0;
      virtual int get_output_size() = 0;
      virtual int get_history();
      virtual float get_shift();
      virtual const char* get_conversion();
      virtual int get_input_item_size();
      virtual int get_output_item_size();
      virtual const char* get_output_conversion();
    };

    FEC_API int get_decoder_output_size(generic_decoder::sptr my_decoder);
    FEC_API int get_decoder_input_size(generic_decoder::sptr my_decoder);
    FEC_API float get_shift(generic_decoder::sptr my_decoder);
    FEC_API int get_history(generic_decoder::sptr my_decoder);
    FEC_API int get_decoder_output_item_size(generic_decoder::sptr my_decoder);
    FEC_API int get_decoder_input_item_size(generic_decoder::sptr my_decoder);
    FEC_API const char* get_conversion(generic_decoder::sptr my_decoder);
    FEC_API const char* get_output_conversion(generic_decoder::sptr my_decoder);

  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_GENRIC_DECODER_H */
