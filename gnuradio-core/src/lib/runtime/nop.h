/* -*- c++ -*- */
/* 
 * Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_CTRLPORT_NOP_H
#define INCLUDED_CTRLPORT_NOP_H

#include <ctrlport/api.h>
#include <gr_sync_block.h>

namespace gr {
  namespace ctrlport {
    
    /*!
     * \brief A NOP block for testing ctrlport
     *
     */
    class CTRLPORT_API nop : virtual public gr_sync_block
    {
    public:
      // gr::ctrlport::nop::sptr
      typedef boost::shared_ptr<nop> sptr;

      /*!
       * Build a simple test block
       */
      static sptr make(size_t itemsize, int a, int b);

      virtual void set_a(int b) = 0;
      virtual void set_b(int b) = 0;
      virtual int a() const = 0;
      virtual int b() const = 0;
    };

  } /* namespace ctrlport */
} /* namespace gr */

#endif /* INCLUDED_CTRLPORT_NOP_H */

