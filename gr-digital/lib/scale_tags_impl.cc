/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
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

#include <gr_io_signature.h>
#include "scale_tags_impl.h"

namespace gr {
  namespace digital {

    scale_tags::sptr
    scale_tags::make(size_t itemsize, const std::string& tagname, float scale_factor)
    {
      return gnuradio::get_initial_sptr (new scale_tags_impl(itemsize, tagname, scale_factor));
    }

    /*
     * The private constructor
     */
    scale_tags_impl::scale_tags_impl(size_t itemsize, const std::string& tagname, float scale_factor)
      : gr_sync_block("scale_tags",
		      gr_make_io_signature(1, 1, itemsize),
                      gr_make_io_signature(1, 1, itemsize)),
        d_itemsize(itemsize),
        d_tagname(tagname),
        d_scale_factor(scale_factor)
    {
      set_tag_propagation_policy(TPP_DONT);
    }

    /*
     * Our virtual destructor.
     */
    scale_tags_impl::~scale_tags_impl()
    {
    }

    int
    scale_tags_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const char *in = (const char *) input_items[0];
        char *out = (char *) output_items[0];
        std::vector<gr_tag_t> tags;
        this->get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0)+noutput_items);
        //const size_t ninput_items = noutput_items; //assumption for sync block, this can change
        for (unsigned int j = 0; j < tags.size(); j++) {
          long value = pmt::pmt_to_long(tags[j].value);
          if (pmt::pmt_symbol_to_string(tags[j].key) == d_tagname) {
            value = long(value*d_scale_factor);
          }
          this->add_item_tag(0, tags[j].offset, tags[j].key, pmt::pmt_from_long(value));
        }
        memcpy((void *) out, (const void *) in, noutput_items*d_itemsize);
        
        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace digital */
} /* namespace gr */

