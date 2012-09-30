/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_HOWTO_SQUARE_FF_H
#define INCLUDED_HOWTO_SQUARE_FF_H

#include <howto/api.h>
#include <gr_block.h>

namespace gr {
  namespace howto {

    /*!
     * \brief square a stream of floats.
     * \ingroup block
     *
     * \sa howto_square2_ff for a version that subclasses gr_sync_block.
     */
    class HOWTO_API square_ff : virtual public gr_block
    {
    public:
      /*
       * We use boost::shared_ptr's instead of raw pointers for all
       * access to gr_blocks (and many other data structures). The
       * shared_ptr gets us transparent reference counting, which
       * greatly simplifies storage management issues. This is
       * especially helpful in our hybrid C++ / Python system.
       *
       * See http://www.boost.org/libs/smart_ptr/smart_ptr.htm
       *
       * gr::howto::square_ff::sptr
       */
       typedef boost::shared_ptr<square_ff> sptr;

       /*!
	* \brief Return a shared_ptr to a new instance of howto::square_ff.
	*
	* To avoid accidental use of raw pointers, howto::square_ff's
	* constructor is in a private implementation
	* class. howto::square_ff::make is the public interface for
	* creating new instances.
	*/
       static sptr make();
    };

  } // namespace howto
} // namespace gr

#endif /* INCLUDED_HOWTO_SQUARE_FF_H */
