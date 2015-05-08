/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DTV_ATSC_RS_DECODER_H
#define INCLUDED_DTV_ATSC_RS_DECODER_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace dtv {

    /*!
     * \brief ATSC Receiver Reed-Solomon Decoder
     *
     * \ingroup dtv_atsc
     */
    class DTV_API atsc_rs_decoder : virtual public gr::sync_block
    {
    public:

      // gr::dtv::atsc_rs_decoder::sptr
      typedef boost::shared_ptr<atsc_rs_decoder> sptr;

      /*!
       * Returns the number of errors corrected by the decoder.
       */
      virtual int num_errors_corrected() const = 0;

      /*!
       * Returns the number of bad packets rejected by the decoder.
       */
      virtual int num_bad_packets() const = 0;

      /*!
       * Returns the total number of packets seen by the decoder.
       */
      virtual int num_packets() const = 0;

      /*!
       * \brief Make a new instance of gr::dtv::atsc_rs_decoder.
       */
      static sptr make();
    };

  } /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_DTV_ATSC_RS_DECODER_H */
