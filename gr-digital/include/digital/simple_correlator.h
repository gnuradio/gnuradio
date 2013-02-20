/* -*- c++ -*- */
/*
 * Copyright 2004,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_SIMPLE_CORRELATOR_H
#define INCLUDED_GR_SIMPLE_CORRELATOR_H

#include <digital/api.h>
#include <gr_block.h>

namespace gr {
  namespace digital {
    
    /*!
     * \brief inverse of simple_framer (more or less)
     * \ingroup sync_blk
     */
    class DIGITAL_API simple_correlator : virtual public gr_block
    {
    public:
      // gr::digital::simple_correlator::sptr
      typedef boost::shared_ptr<simple_correlator> sptr;

      static sptr make(int payload_bytesize);
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_SIMPLE_CORRELATOR_H */
