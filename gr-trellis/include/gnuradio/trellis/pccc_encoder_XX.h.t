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

// @WARNING@

#ifndef @GUARD_NAME@
#define @GUARD_NAME@

#include <gnuradio/trellis/api.h>
#include <gnuradio/trellis/fsm.h>
#include <gnuradio/trellis/interleaver.h>
#include <gnuradio/sync_block.h>
#include <vector>

namespace gr {
  namespace trellis {

    /*!
     * \brief PCCC encoder.
     * \ingroup trellis_coding_blk
     */
    class TRELLIS_API @NAME@ : virtual public sync_block
    {
    public:
      // gr::trellis::@BASE_NAME@::sptr
      typedef boost::shared_ptr<@BASE_NAME@> sptr;

      static sptr make(const fsm &FSM1, int ST1,
		       const fsm &FSM2, int ST2,
		       const interleaver &INTERLEAVER,
		       int blocklength);

      virtual fsm FSM1() const = 0;
      virtual int ST1() const = 0;
      virtual fsm FSM2() const = 0;
      virtual int ST2() const = 0;
      virtual interleaver INTERLEAVER() const = 0;
      virtual int blocklength() const = 0;
    };

  } /* namespace trellis */
} /* namespace gr */

#endif /* @GUARD_NAME@ */
