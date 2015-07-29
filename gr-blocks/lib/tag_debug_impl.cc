/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
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

#include "tag_debug_impl.h"
#include <gnuradio/io_signature.h>
#include <iostream>
#include <iomanip>

namespace gr {
  namespace blocks {

    tag_debug::sptr
    tag_debug::make(size_t sizeof_stream_item,
                    const std::string &name,
                    const std::string &key_filter)
    {
      return gnuradio::get_initial_sptr
        (new tag_debug_impl(sizeof_stream_item, name, key_filter));
    }

    tag_debug_impl::tag_debug_impl(size_t sizeof_stream_item,
                                   const std::string &name,
                                   const std::string &key_filter)
      : sync_block("tag_debug",
                   io_signature::make(1, -1, sizeof_stream_item),
                   io_signature::make(0, 0, 0)),
        d_name(name), d_display(true)
    {
      set_key_filter(key_filter);
    }

    tag_debug_impl::~tag_debug_impl()
    {
    }

    std::vector<tag_t>
    tag_debug_impl::current_tags()
    {
      gr::thread::scoped_lock l(d_mutex);
      return d_tags;
    }

    int
    tag_debug_impl::num_tags()
    {
      std::vector<tag_t> t;
      get_tags_in_range(t, 0, 0, nitems_read(0));
      return static_cast<int>(t.size());
    }

    void
    tag_debug_impl::set_display(bool d)
    {
      d_display = d;
    }

    void
    tag_debug_impl::set_key_filter(const std::string &key_filter)
    {
      if(key_filter == "")
        d_filter = pmt::PMT_NIL;
      else
        d_filter = pmt::intern(key_filter);
    }

    std::string
    tag_debug_impl::key_filter() const
    {
      return pmt::symbol_to_string(d_filter);
    }

    int
    tag_debug_impl::work(int noutput_items,
                         gr_vector_const_void_star &input_items,
                         gr_vector_void_star &output_items)
    {
      gr::thread::scoped_lock l(d_mutex);
      bool toprint = false;

      std::stringstream sout;
      if(d_display) {
        sout << std::endl
             << "----------------------------------------------------------------------";
        sout << std::endl << "Tag Debug: " << d_name << std::endl;
      }

      uint64_t abs_N, end_N;
      for(size_t i = 0; i < input_items.size(); i++) {
        abs_N = nitems_read(i);
        end_N = abs_N + (uint64_t)(noutput_items);

        d_tags.clear();
        if(pmt::is_null(d_filter))
          get_tags_in_range(d_tags, i, abs_N, end_N);
        else
          get_tags_in_range(d_tags, i, abs_N, end_N, d_filter);

        if(d_tags.size() > 0) {
          toprint = true;
        }

        if(d_display) {
          sout << "Input Stream: " << std::setw(2) << std::setfill('0')
               << i << std::setfill(' ') << std::endl;
          for(d_tags_itr = d_tags.begin(); d_tags_itr != d_tags.end(); d_tags_itr++) {
            sout << std::setw(10) << "Offset: " << d_tags_itr->offset
                 << std::setw(10) << "Source: "
                 << (pmt::is_symbol(d_tags_itr->srcid) ? pmt::symbol_to_string(d_tags_itr->srcid) : "n/a")
                 << std::setw(10) << "Key: " << pmt::symbol_to_string(d_tags_itr->key)
                 << std::setw(10) << "Value: ";
            sout << d_tags_itr->value << std::endl;
          }
        }
      }

      if(d_display) {
        sout << "----------------------------------------------------------------------";
        sout << std::endl;

        if(toprint) {
          std::cout << sout.str();
        }
      }

      return noutput_items;
    }

    void
    tag_debug_impl::setup_rpc()
    {
#ifdef GR_CTRLPORT
      add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_get<tag_debug, int>(
	  alias(), "num. tags",
	  &tag_debug::num_tags,
	  pmt::from_long(0), pmt::from_long(10000), pmt::from_long(0),
	  "", "Number of Tags", RPC_PRIVLVL_MIN,
          DISPTIME | DISPOPTSTRIP)));
#endif /* GR_CTRLPORT */
    }

  } /* namespace blocks */
} /* namespace gr */
