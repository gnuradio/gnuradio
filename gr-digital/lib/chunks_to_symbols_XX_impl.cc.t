/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012 Free Software Foundation, Inc.
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

/* @WARNING@ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "@NAME@.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/tag_checker.h>
#include <assert.h>

namespace gr {
  namespace digital {

    @BASE_NAME@::sptr
    @BASE_NAME@::make(const std::vector<@O_TYPE@> &symbol_table, const int D)
    {
      return gnuradio::get_initial_sptr
        (new @IMPL_NAME@(symbol_table, D));
    }

    @IMPL_NAME@::@IMPL_NAME@(const std::vector<@O_TYPE@> &symbol_table, const int D)
    : sync_interpolator("@BASE_NAME@",
			   io_signature::make(1, -1, sizeof(@I_TYPE@)),
			   io_signature::make(1, -1, sizeof(@O_TYPE@)),
			   D),
      d_D(D), d_symbol_table(symbol_table)
    {
      message_port_register_in(pmt::mp("set_symbol_table"));
      set_msg_handler(
        pmt::mp("set_symbol_table"),
        boost::bind(&@IMPL_NAME@::handle_set_symbol_table,
                    this, _1));
    }

    @IMPL_NAME@::~@IMPL_NAME@()
    {
    }


    void
    @IMPL_NAME@::set_vector_from_pmt(std::vector<gr_complex> &symbol_table, pmt::pmt_t &symbol_table_pmt) {
      size_t length;
      const gr_complex *elements = pmt::c32vector_elements(symbol_table_pmt, length);
      symbol_table.assign(elements, elements + length);
    }

    void
    @IMPL_NAME@::set_vector_from_pmt(std::vector<float> &symbol_table, pmt::pmt_t &symbol_table_pmt) {
      size_t length;
      const float *elements = pmt::f32vector_elements(symbol_table_pmt, length);
      symbol_table.assign(elements, elements + length);
    }

    void
    @IMPL_NAME@::handle_set_symbol_table(pmt::pmt_t symbol_table_pmt)
    {
      set_vector_from_pmt(d_symbol_table, symbol_table_pmt);
    }

    void
    @IMPL_NAME@::set_symbol_table(const std::vector<@O_TYPE@> &symbol_table)
    {
      gr::thread::scoped_lock lock(d_setlock);
      d_symbol_table = symbol_table;
    }

    int
    @IMPL_NAME@::work(int noutput_items,
		      gr_vector_const_void_star &input_items,
		      gr_vector_void_star &output_items)
    {
      gr::thread::scoped_lock lock(d_setlock);
      assert(noutput_items % d_D == 0);
      assert(input_items.size() == output_items.size());
      int nstreams = input_items.size();

      for(int m = 0; m < nstreams; m++) {
        const @I_TYPE@ *in = (@I_TYPE@*)input_items[m];
        @O_TYPE@ *out = (@O_TYPE@*)output_items[m];

        std::vector<tag_t> tags;
        get_tags_in_range(tags, m, nitems_read(m), nitems_read(m)+noutput_items/d_D);
        tag_checker tchecker(tags);

        // per stream processing
        for(int i = 0; i < noutput_items / d_D; i++) {
          std::vector<tag_t> tags_now;
          tchecker.get_tags(tags_now, i+nitems_read(m));
          for (unsigned int j=0; j<tags_now.size(); j++) {
            tag_t tag = tags_now[j];
            dispatch_msg(tag.key, tag.value);
          }
          assert(((unsigned int)in[i]*d_D+d_D) <= d_symbol_table.size());
          memcpy(out, &d_symbol_table[(unsigned int)in[i]*d_D], d_D*sizeof(@O_TYPE@));
          out+=d_D;
        }
      }
      return noutput_items;
    }

  } /* namespace digital */
} /* namespace gr */
