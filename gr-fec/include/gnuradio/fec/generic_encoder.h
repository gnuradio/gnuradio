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

#ifndef INCLUDED_FEC_GENERIC_ENCODER_H
#define INCLUDED_FEC_GENERIC_ENCODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/block.h>
#include <boost/shared_ptr.hpp>

namespace gr {
  namespace fec {

    class FEC_API generic_encoder
    {
    public:
      friend class encoder;
      virtual void generic_work(void *inBuffer, void *outBuffer) = 0;
    public:
      typedef boost::shared_ptr<generic_encoder> sptr;

      virtual int get_input_size() = 0;
      virtual int get_output_size() = 0;
      generic_encoder(void) {};
      virtual ~generic_encoder();
    };

    FEC_API int get_encoder_output_size(generic_encoder::sptr my_encoder);
    FEC_API int get_encoder_input_size(generic_encoder::sptr my_encoder);

  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_GENERIC_ENCODER_H */
