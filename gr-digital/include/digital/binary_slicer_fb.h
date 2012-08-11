/* -*- c++ -*- */
/*
 * Copyright 2006,2011,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_BINARY_SLICER_FB_H
#define INCLUDED_DIGITAL_BINARY_SLICER_FB_H

#include <digital/api.h>
#include <gr_sync_block.h>

namespace gr {
  namespace digital {
    
    /*!
     * \brief slice float binary symbol outputting 1 bit output
     * \ingroup converter_blk
     * \ingroup digital
     *
     * x <  0 --> 0
     * x >= 0 --> 1
     */
    class DIGITAL_API binary_slicer_fb : virtual public gr_sync_block
    {
    public:
      // gr::digital::binary_slicer_fb::sptr
      typedef boost::shared_ptr<binary_slicer_fb> sptr;

      /*!
       * \brief Make binary symbol slicer block.
       */
      static sptr make();
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_BINARY_SLICER_FB_H */
