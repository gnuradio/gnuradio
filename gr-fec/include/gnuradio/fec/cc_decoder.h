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

#ifndef INCLUDED_FEC_CC_DECODER_H
#define INCLUDED_FEC_CC_DECODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/generic_decoder.h>
#include <map>
#include <string>
//#include <fec/cc_common.h>

namespace gr {
  namespace fec {
    namespace code {

    typedef void(*conv_kernel)(unsigned char  *Y, unsigned char  *X,
                               unsigned char *syms, unsigned char *dec,
                               unsigned int framebits, unsigned int excess,
                               unsigned char  *Branchtab);

    class FEC_API cc_decoder : virtual public generic_decoder
    {
    public:
      static generic_decoder::sptr make
        (int framebits, int k,
         int rate, std::vector<int> polys,
         int start_state = 0, int end_state = -1,
         bool tailbiting = false, bool terminated = false,
         bool truncated = false, bool streaming = false);

      virtual void set_framebits(int framebits) = 0;
      virtual void generic_work(void *inBuffer, void *outbuffer) = 0;
    };

    } /* namespace code */
  } /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_CC_DECODER_H */
