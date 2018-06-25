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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "dvbt_demod_reference_signals_impl.h"

namespace gr {
  namespace dtv {

    dvbt_demod_reference_signals::sptr
    dvbt_demod_reference_signals::make(int itemsize, int ninput, int noutput, \
        dvb_constellation_t constellation, dvbt_hierarchy_t hierarchy, \
        dvb_code_rate_t code_rate_HP, dvb_code_rate_t code_rate_LP, \
        dvb_guardinterval_t guard_interval, dvbt_transmission_mode_t transmission_mode, \
        int include_cell_id, int cell_id)
    {
      return gnuradio::get_initial_sptr
        (new dvbt_demod_reference_signals_impl(itemsize, ninput, \
            noutput, constellation, hierarchy, code_rate_HP, code_rate_LP, \
            guard_interval, transmission_mode, include_cell_id, cell_id));
    }

    /*
     * The private constructor
     */
    dvbt_demod_reference_signals_impl::dvbt_demod_reference_signals_impl(int itemsize, int ninput, int noutput, \
        dvb_constellation_t constellation, dvbt_hierarchy_t hierarchy, dvb_code_rate_t code_rate_HP,\
          dvb_code_rate_t code_rate_LP, dvb_guardinterval_t guard_interval,\
          dvbt_transmission_mode_t transmission_mode, int include_cell_id, int cell_id)
      : block("dvbt_demod_reference_signals",
          io_signature::make(1, 1, itemsize * ninput),
          io_signature::make(1, 1, itemsize * noutput)),
          config(constellation, hierarchy, code_rate_HP, code_rate_LP, \
            guard_interval, transmission_mode, include_cell_id, cell_id),
          d_pg(config),
          d_init(0),
          d_fi_start(0)
    {
      d_ninput = ninput;
      d_noutput = noutput;

      // TODO - investigate why this is happening
      if ((config.d_constellation == MOD_64QAM) && (config.d_transmission_mode == T8k))
        d_fi_start = 2;
      else
        d_fi_start = 3;
    }

    /*
     * Our virtual destructor.
     */
    dvbt_demod_reference_signals_impl::~dvbt_demod_reference_signals_impl()
    {
    }

    void
    dvbt_demod_reference_signals_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      int ninputs = ninput_items_required.size();

      for (int i = 0; i < ninputs; i++)
        ninput_items_required[i] =  2 * noutput_items;
    }

    int
    dvbt_demod_reference_signals_impl::is_sync_start(int nitems)
    {
      std::vector<tag_t> tags;
      const uint64_t nread = this->nitems_read(0); //number of items read on port 0
      this->get_tags_in_range(tags, 0, nread, nread + nitems, pmt::string_to_symbol("sync_start"));

      return tags.size() ? 1 : 0;
    }

    int
    dvbt_demod_reference_signals_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      int symbol_index = 0;
      int frame_index = 0;
      int to_out = 0;

      for (int i = 0; i < noutput_items; i++) {
        to_out += d_pg.parse_input(&in[i * d_ninput], &out[i * d_noutput], &symbol_index, &frame_index);
      }

      /*
       * Wait for a sync_start tag from upstream that signals when to start.
       * Always consume until to a superframe start.
       */
      if (is_sync_start(noutput_items)) {
        d_init = 0;
      }

      if (d_init == 0) {
        // This is super-frame start
        if (((symbol_index % 68) == 0) && ((frame_index % 4) == d_fi_start)) {
          d_init = 1;

          const uint64_t offset = this->nitems_written(0);
          pmt::pmt_t key = pmt::string_to_symbol("superframe_start");
          pmt::pmt_t value = pmt::from_long(0xaa);
          this->add_item_tag(0, offset, key, value);
        }
        else {
          consume_each(1);
          return (0);
        }
      }

      // Send a tag for each OFDM symbol informing about
      // symbol index.
      const uint64_t offset = this->nitems_written(0);
      pmt::pmt_t key = pmt::string_to_symbol("symbol_index");
      pmt::pmt_t value = pmt::from_long(symbol_index);
      this->add_item_tag(0, offset, key, value);

      // Consume from input stream
      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return to_out;
    }

  } /* namespace dtv */
} /* namespace gr */

