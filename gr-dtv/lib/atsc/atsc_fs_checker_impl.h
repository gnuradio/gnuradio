/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DTV_ATSC_FS_CHECKER_IMPL_H
#define INCLUDED_DTV_ATSC_FS_CHECKER_IMPL_H

#include <gnuradio/dtv/atsc_fs_checker.h>
#include "atsc_syminfo_impl.h"

namespace gr {
  namespace dtv {

    class atsc_fs_checker_impl : public atsc_fs_checker
    {
    private:
      static const int	SRSIZE = 1024;		// must be power of two
      int			d_index;		// points at oldest sample
      float			d_sample_sr[SRSIZE];	// sample shift register
      atsc::syminfo		d_tag_sr[SRSIZE];	// tag shift register
      unsigned char		d_bit_sr[SRSIZE];	// binary decision shift register
      int			d_field_num;
      int			d_segment_num;

      static const int	OFFSET_511 = 4;	// offset to second PN 63 pattern
      static const int	LENGTH_511 = 511;	// length of PN 63 pattern
      static const int	OFFSET_2ND_63 = 578;	// offset to second PN 63 pattern
      static const int	LENGTH_2ND_63 = 63;	// length of PN 63 pattern

      inline static int wrap (int index){ return index & (SRSIZE - 1); }
      inline static int incr (int index){ return wrap (index + 1); }
      inline static int decr (int index){ return wrap (index - 1); }

    public:
      atsc_fs_checker_impl();
      ~atsc_fs_checker_impl();

      void reset();

      virtual int general_work(int noutput_items,
                               gr_vector_int &ninput_items,
                               gr_vector_const_void_star &input_items,
                               gr_vector_void_star &output_items);
    };

  } /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_DTV_ATSC_FS_CHECKER_IMPL_H */
