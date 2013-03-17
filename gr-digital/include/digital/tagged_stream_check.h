/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_TAGGED_STREAM_CHECK_H
#define INCLUDED_DIGITAL_TAGGED_STREAM_CHECK_H

//#include <digital_ofdm_equalizer_base.h> // FIXME: Error without this line. No idea why.
#include <digital/api.h>
#include <gr_sync_block.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Checks a tagged stream to make sure it's valid.
     *
     * \description
     * Produces error messages if the tags in a tagged stream aren't where
     * they should be.
     *
     * \ingroup digital
     *
     */
    class DIGITAL_API tagged_stream_check : virtual public gr_sync_block
    {
    public:
      typedef boost::shared_ptr<tagged_stream_check> sptr;
      static sptr make(size_t itemsize, const std::string &lengthtagname);
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_TAGGED_STREAM_CHECK_H */

