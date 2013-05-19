/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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

#ifndef INCLUDED_PAGER_FLEX_SYNC_H
#define INCLUDED_PAGER_FLEX_SYNC_H

#include <gnuradio/pager/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace pager {

    /*!
     * \brief flex sync description
     * \ingroup pager_blk
     */
    class PAGER_API flex_sync : virtual public block
    {
    public:
      // gr::pager::flex_sync::sptr
      typedef boost::shared_ptr<flex_sync> sptr;

      static sptr make();

    };

  } /* namespace pager */
} /* namespace gr */

#endif /* INCLUDED_PAGER_FLEX_SYNC_H */
