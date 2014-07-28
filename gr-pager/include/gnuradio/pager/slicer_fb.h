/* -*- c++ -*- */
/*
 * Copyright 2006,2007,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_PAGER_SLICER_FB_H
#define INCLUDED_PAGER_SLICER_FB_H

#include <gnuradio/pager/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace pager {

    /*!
     * \brief slicer description
     * \ingroup pager_blk
     */
    class PAGER_API slicer_fb : virtual public sync_block
    {
    public:
      // gr::pager::slicer_fb::sptr
      typedef boost::shared_ptr<slicer_fb> sptr;

      /*!
       * \brief Make a pager slicer
       */
      static sptr make(float alpha);

      virtual float dc_offset() const = 0;
    };

  } /* namespace pager */
} /* namespace gr */

#endif /* INCLUDED_PAGER_SLICER_FB_H */
