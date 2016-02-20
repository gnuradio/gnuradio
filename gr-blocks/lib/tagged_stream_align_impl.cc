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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "tagged_stream_align_impl.h"

namespace gr {
  namespace blocks {

    tagged_stream_align::sptr
    tagged_stream_align::make(size_t itemsize, const std::string &lengthtagname)
    {
      return gnuradio::get_initial_sptr (new tagged_stream_align_impl(itemsize, lengthtagname));
    }

    tagged_stream_align_impl::tagged_stream_align_impl(size_t itemsize, const std::string &lengthtagname)
      : block("tagged_stream_align",
                 io_signature::make(1,  1, itemsize),
                 io_signature::make(1,  1, itemsize)),
        d_itemsize(itemsize),
        d_lengthtag(pmt::mp(lengthtagname)),
        d_have_sync(false)
    {
      set_tag_propagation_policy(TPP_DONT);
    }

    tagged_stream_align_impl::~tagged_stream_align_impl()
    {
    }

    int 
    tagged_stream_align_impl::general_work(int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      std::vector<tag_t> tags;
      if(d_have_sync){
        int ncp = std::min(noutput_items, ninput_items[0]);
        get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + noutput_items);
        for(size_t i=0; i<tags.size(); i++){
            gr::tag_t t = tags[i];
            int offset = (nitems_read(0) - nitems_written(0));
            t.offset -= offset;
            add_item_tag(0,t);
            }
        memcpy(output_items[0], input_items[0], ncp*d_itemsize);
        consume_each(ncp);
        return ncp;
      } else {
        get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + ninput_items[0], d_lengthtag);
        if(tags.size() > 0){
            d_have_sync = true;
            consume_each( tags[0].offset - nitems_read(0) );
        } else {
            consume_each(ninput_items[0]);
        }
        return 0;
        }
    }

  } /* namespace blocks */
} /* namespace gr */

