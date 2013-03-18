/* -*- c++ -*- */
/* 
 * Copyright 2012 <+YOU OR YOUR COMPANY+>.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_io_signature.h>
#include "tagged_stream_check_impl.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>

namespace gr {
  namespace digital {

    bool sort_tag_by_offset(gr_tag_t const & L, gr_tag_t const & R) {
      return L.offset < R.offset;
    }

    tagged_stream_check::sptr
    tagged_stream_check::make(size_t itemsize, const std::string &lengthtagname)
    {
      return gnuradio::get_initial_sptr (new tagged_stream_check_impl(itemsize, lengthtagname));
    }

    /*
     * The private constructor
     */
    tagged_stream_check_impl::tagged_stream_check_impl(size_t itemsize, const std::string &lengthtagname)
      : gr_sync_block("tagged_stream_check",
                      gr_make_io_signature(1, 1, itemsize),
                      gr_make_io_signature(1, 1, itemsize)),
        d_lengthtagname(lengthtagname), d_itemsize(itemsize)
    {
      d_expected_offset = 0;
      d_last_offset = 0;
    }

    /*
     * Our virtual destructor.
     */
    tagged_stream_check_impl::~tagged_stream_check_impl()
    {
    }

    int
    tagged_stream_check_impl::work (int noutput_items,
                                    gr_vector_const_void_star &input_items,
                                    gr_vector_void_star &output_items)
    {
      char *out = (char *) output_items[0];
      const char *in = (const char*) input_items[0];
      // Find all the length tags
      std::vector<gr_tag_t> tags;
      std::vector<gr_tag_t> lengthtags;
      this->get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0)+noutput_items);
      for (unsigned int j = 0; j < tags.size(); j++) {
        if (pmt::symbol_to_string(tags[j].key) == d_lengthtagname) {
          lengthtags.push_back(tags[j]);
        }
      }
      // If there are no lengthtags then check that we weren't expecting one.
      if (lengthtags.size() == 0) {
        if (d_expected_offset < nitems_read(0)+noutput_items) {
          std::ostringstream ss;
          ss << "ERROR: "<<this->unique_id()<<" Expected a lengthtag at offset="<<d_expected_offset<<" but didn't find it";
          std::cout << ss.str() << std::endl;
        }
      }
      // Sort them and make sure the lengthtags are in the proper places.
      sort(lengthtags.begin(), lengthtags.end(), sort_tag_by_offset);
      for (unsigned int j = 0; j < lengthtags.size(); j++) {
        if (lengthtags[j].offset != d_expected_offset) {
          std::cout << "****************************" << std::endl;
          std::cout << "ERROR: "<<this->unique_id()<<" offset: " << lengthtags[j].offset << " The last tag had a length of " << d_expected_offset - d_last_offset << " but we got a length of " << lengthtags[j].offset - d_last_offset << std::endl;
        }
        long packet_length = pmt::to_long(lengthtags[j].value);
        std::cout << "INFO: "<<this->unique_id()<<" offset: " << lengthtags[j].offset << std::endl;
        d_expected_offset = lengthtags[j].offset + packet_length;
        d_last_offset = lengthtags[j].offset;
      }
      memcpy((void *) out, (const void *) in, noutput_items*d_itemsize);
      std::ostringstream ss;
      ss << "checker: Produced data from " << nitems_read(0) << " to " << nitems_read(0) + noutput_items;
      std::cout << ss.str() << std::endl;
      return noutput_items;
    }


  } /* namespace digital */
} /* namespace gr */

