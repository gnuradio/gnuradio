/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_DTV_ATSC_RANDOMIZER_IMPL_H
#define INCLUDED_DTV_ATSC_RANDOMIZER_IMPL_H

#include <gnuradio/dtv/atsc_randomizer.h>
#include "atsc_randomize.h"
#include "atsc_types.h"

namespace gr {
  namespace dtv {

    class atsc_randomizer_impl : public atsc_randomizer
    {
    private:
      atsc_randomize d_rand;
      int d_segno;
      bool d_field2;

      void reset(void);

    public:
      atsc_randomizer_impl();
      ~atsc_randomizer_impl();

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_ATSC_RANDOMIZER_IMPL_H */
