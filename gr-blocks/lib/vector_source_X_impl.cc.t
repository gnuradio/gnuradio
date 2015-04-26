/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2010,2013 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <@NAME_IMPL@.h>
#include <algorithm>
#include <gnuradio/io_signature.h>
#include <stdexcept>

namespace gr {
  namespace blocks {

    @NAME@::sptr
    @NAME@::make(const std::vector<@TYPE@> &data,
                 bool repeat, int vlen,
                 const std::vector<tag_t> &tags)
    {
      return gnuradio::get_initial_sptr
        (new @NAME_IMPL@(data, repeat, vlen, tags));
    }

    @NAME_IMPL@::@NAME_IMPL@(const std::vector<@TYPE@> &data,
                             bool repeat, int vlen,
                             const std::vector<tag_t> &tags)
    : sync_block("@BASE_NAME@",
                    io_signature::make(0, 0, 0),
                    io_signature::make(1, 1, sizeof(@TYPE@) * vlen)),
      d_data(data),
      d_repeat(repeat),
      d_offset(0),
      d_vlen(vlen),
      d_tags(tags),
      d_tagpos(0)
    {
      if(tags.size() == 0) {
        d_settags = 0;
      }
      else {
        d_settags = 1;
        set_output_multiple(data.size() / vlen);
      }
      if((data.size() % vlen) != 0)
        throw std::invalid_argument("data length must be a multiple of vlen");
    }

    @NAME_IMPL@::~@NAME_IMPL@()
    {}

    void
    @NAME_IMPL@::set_data (const std::vector<@TYPE@> &data,
                           const std::vector<tag_t> &tags)
    {
      d_data = data;
      d_tags = tags;
      rewind();
      if(tags.size() == 0) {
        d_settags = false;
      }
      else {
        d_settags = true;
      }
    }

    int
    @NAME_IMPL@::work(int noutput_items,
                      gr_vector_const_void_star &input_items,
                      gr_vector_void_star &output_items)
    {
      @TYPE@ *optr = (@TYPE@ *) output_items[0];

      if(d_repeat) {
        unsigned int size = d_data.size ();
        unsigned int offset = d_offset;
        if(size == 0)
          return -1;

        if(d_settags) {
          int n_outputitems_per_vector = d_data.size() / d_vlen;
          for(int i = 0; i < noutput_items; i += n_outputitems_per_vector) {
            // FIXME do proper vector copy
            memcpy((void *) optr, (const void*)&d_data[0], size*sizeof (@TYPE@));
            optr += size;
            for(unsigned t = 0; t < d_tags.size(); t++) {
              add_item_tag(0, nitems_written(0)+i+d_tags[t].offset,
                           d_tags[t].key, d_tags[t].value, d_tags[t].srcid);
            }
          }
        }
        else {
          for(int i = 0; i < noutput_items*d_vlen; i++) {
            optr[i] = d_data[offset++];
            if(offset >= size) {
              offset = 0;
            }
          }
        }

        d_offset = offset;
        return noutput_items;
      }
      else {
        if(d_offset >= d_data.size ())
          return -1;  // Done!

        unsigned n = std::min((unsigned)d_data.size() - d_offset,
                              (unsigned)noutput_items*d_vlen);
        for(unsigned i = 0; i < n; i++) {
          optr[i] = d_data[d_offset + i];
        }
        for(unsigned t = 0; t < d_tags.size(); t++) {
          if((d_tags[t].offset >= d_offset) && (d_tags[t].offset < d_offset+n))
            add_item_tag(0, d_tags[t].offset, d_tags[t].key, d_tags[t].value,
                         d_tags[t].srcid);
        }
        d_offset += n;
        return n/d_vlen;
      }
    }

  } /* namespace blocks */
} /* namespace gr */
