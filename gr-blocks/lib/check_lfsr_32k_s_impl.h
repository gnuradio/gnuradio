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

#ifndef INCLUDED_GR_CHECK_LFSR_32K_S_IMPL_H
#define INCLUDED_GR_CHECK_LFSR_32K_S_IMPL_H

#include <gnuradio/blocks/check_lfsr_32k_s.h>
#include <gnuradio/blocks/lfsr_32k.h>

namespace gr {
  namespace blocks {

    class check_lfsr_32k_s_impl : public check_lfsr_32k_s
    {
    private:
      enum state {
        SEARCHING,	// searching for synchronization
        MATCH0,
        MATCH1,
        MATCH2,
        LOCKED		// is locked
      };

      state d_state;
      unsigned int d_history;  // bitmask of decisions

      long d_ntotal;     // total number of shorts
      long d_nright;     // # of correct shorts
      long d_runlength;  // # of correct shorts in a row

      static const int BUFSIZE = 2048 - 1;  // ensure pattern isn't packet aligned
      int d_index;
      unsigned short d_buffer[BUFSIZE];

      void enter_SEARCHING();
      void enter_MATCH0();
      void enter_MATCH1();
      void enter_MATCH2();
      void enter_LOCKED();

      void right()
      {
        d_history = (d_history << 1) | 0x1;
        d_nright++;
        d_runlength++;
      }

      void wrong()
      {
        d_history = (d_history << 1) | 0x0;
        d_runlength = 0;
      }

      bool right_three_times() { return (d_history & 0x7) == 0x7; }
      bool wrong_three_times() { return (d_history & 0x7) == 0x0; }

      void log_error(unsigned short expected, unsigned short actual);

    public:
      check_lfsr_32k_s_impl();
      ~check_lfsr_32k_s_impl();

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);

      long ntotal() const { return d_ntotal; }
      long nright() const { return d_nright; }
      long runlength() const { return d_runlength; }
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_CHECK_LFSR_32K_S_IMPL_H */
