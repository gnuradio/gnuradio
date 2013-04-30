/* -*- c++ -*- */
/*
 * Copyright 2009,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_NOAA_HRPT_DECODER_H
#define INCLUDED_NOAA_HRPT_DECODER_H

#include <gnuradio/noaa/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace noaa {

    /*!
     * \brief NOAA HRPT Decoder
     * \ingroup noaa_blk
     */
    class NOAA_API hrpt_decoder : virtual public sync_block
    {
    public:
      // gr::noaa::hrpt_decoder::sptr
      typedef boost::shared_ptr<hrpt_decoder> sptr;

      /*!
       * \brief Make NOAA HRPT Decoder
       */
      static sptr make(bool verbose, bool output_files);
    };

  } /* namespace noaa */
} /* namespace gr */

#endif /* INCLUDED_NOAA_HRPT_DECODER_H */
