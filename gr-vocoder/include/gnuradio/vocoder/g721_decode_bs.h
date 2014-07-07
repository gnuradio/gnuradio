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

#ifndef INCLUDED_VOCODER_G721_DECODE_BS_H
#define INCLUDED_VOCODER_G721_DECODE_BS_H

#include <gnuradio/vocoder/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace vocoder {

    /*!
     * \brief This block performs g721 audio decoding.
     * \ingroup audio_blk
     */
    class VOCODER_API g721_decode_bs : virtual public sync_block
    {
    public:
      // gr::vocoder::g721_decode_bs::sptr
      typedef boost::shared_ptr<g721_decode_bs> sptr;

      /*!
       * \brief Make G721 decoder block.
       */
      static sptr make();
    };

  } /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_G721_DECODE_BS_H */
