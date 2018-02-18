/* -*- c++ -*- */
/*
 * Copyright 2010,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_ANNOTATOR_ALLTOALL_IMPL_H
#define	INCLUDED_GR_ANNOTATOR_ALLTOALL_IMPL_H

#include <gnuradio/blocks/annotator_alltoall.h>

namespace gr {
  namespace blocks {

    class annotator_alltoall_impl : public annotator_alltoall
    {
    private:
      uint64_t d_when;
      uint64_t d_tag_counter;
      std::vector<tag_t> d_stored_tags;

    public:
      annotator_alltoall_impl(int when, size_t sizeof_stream_item);
      ~annotator_alltoall_impl();

      std::vector<tag_t> data() const
      {
        return d_stored_tags;
      }

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_ANNOTATOR_ALLTOALL_IMPL_H */
