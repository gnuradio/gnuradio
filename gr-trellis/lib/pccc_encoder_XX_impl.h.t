/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#include <gnuradio/trellis/@BASE_NAME@.h>

namespace gr {
  namespace trellis {

    class @IMPL_NAME@ : public @BASE_NAME@
    {
    private:
      fsm d_FSM1;
      int d_ST1;
      fsm d_FSM2;
      int d_ST2;
      interleaver d_INTERLEAVER;
      int d_blocklength;
      std::vector<int> d_buffer;

    public:
      @IMPL_NAME@(const fsm &FSM1, int ST1,
		  const fsm &FSM2, int ST2,
		  const interleaver &INTERLEAVER,
		  int blocklength);
      ~@IMPL_NAME@();

      fsm FSM1() const { return d_FSM1; }
      int ST1() const { return d_ST1; }
      fsm FSM2() const { return d_FSM2; }
      int ST2() const { return d_ST2; }
      interleaver INTERLEAVER() const { return d_INTERLEAVER; }
      int blocklength() const { return d_blocklength; }

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace trellis */
} /* namespace gr */

#endif /* @GUARD_NAME@ */
