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

#ifndef INCLUDED_DIGITAL_TS_INSERT_ZEROS_CC_H
#define INCLUDED_DIGITAL_TS_INSERT_ZEROS_CC_H

//#include <digital_ofdm_equalizer_base.h> // FIXME: Error without this line. No idea why.
#include <digital/api.h>
#include <gr_block.h>
#include <string>

namespace gr {
  namespace digital {

    /*!
     * \brief Inserts zeros between the packets of a tagged stream.
     *
     * \description
     * Outputs zeros if a packet is not yet ready at the input, otherwise
     * passes packets through.
     *
     * \ingroup digital
     *
     */
    class DIGITAL_API ts_insert_zeros_cc : virtual public gr_block
    {
    public:
      typedef boost::shared_ptr<ts_insert_zeros_cc> sptr;
      static sptr make(const std::string &lengthtagname);
    };
  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_TS_INSERT_ZEROS_CC_H */

