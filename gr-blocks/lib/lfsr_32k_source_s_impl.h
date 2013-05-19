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

#ifndef INCLUDED_GR_LFSR_32K_SOURCE_S_IMPL_H
#define INCLUDED_GR_LFSR_32K_SOURCE_S_IMPL_H

#include <gnuradio/blocks/lfsr_32k_source_s.h>
#include <gnuradio/blocks/lfsr_32k.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    class lfsr_32k_source_s_impl : public lfsr_32k_source_s
    {
    private:
      static const int BUFSIZE = 2048 - 1;	// ensure pattern isn't packet aligned
      int d_index;
      short d_buffer[BUFSIZE];

    public:
      lfsr_32k_source_s_impl();
      ~lfsr_32k_source_s_impl();

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_LFSR_32K_SOURCE_S_IMPL_H */
