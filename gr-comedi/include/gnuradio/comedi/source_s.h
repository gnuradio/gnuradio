/* -*- c++ -*- */
/*
 * Copyright 2005,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_COMEDI_SOURCE_S_H
#define INCLUDED_COMEDI_SOURCE_S_H

#include <gnuradio/comedi/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace comedi {

    /*!
     * \brief source using COMEDI
     *
     * The source has one to many input stream of signed short integers.
     *
     * Output samples will be in the range [-32768,32767].
     */
    class COMEDI_API source_s : virtual public sync_block
    {
    public:
      // gr::comedi::source_s::sptr
      typedef boost::shared_ptr<source_s> sptr;

      /*!
       * \brief make a COMEDI source.
       *
       * \param sampling_freq sampling rate in Hz
       * \param dev COMEDI device name, e.g., "/dev/comedi0"
       */
      static sptr make(int sampling_freq,
		       const std::string dev = "/dev/comedi0");
    };

  } /* namespace comedi */
} /* namespace gr */

#endif /* INCLUDED_COMEDI_SOURCE_S_H */
