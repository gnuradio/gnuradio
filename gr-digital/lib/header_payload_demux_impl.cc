/* -*- c++ -*- */
/* Copyright 2012,2013 Free Software Foundation, Inc.
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

#include <climits>
#include <boost/format.hpp>
#include <gnuradio/io_signature.h>
#include "header_payload_demux_impl.h"


namespace gr {
  namespace digital {

    enum demux_states_t {
      STATE_FIND_TRIGGER,       // "Idle" state (waiting for burst)
      STATE_HEADER,             // Copy header
      STATE_WAIT_FOR_MSG,       // Null state (wait until msg from header demod)
      STATE_HEADER_RX_SUCCESS,  // Header processing
      STATE_HEADER_RX_FAIL,     //       "
      STATE_PAYLOAD             // Copy payload
    };

#define msg_port_id pmt::mp("header_data")

    header_payload_demux::sptr
    header_payload_demux::make(
	int header_len,
	int items_per_symbol,
	int guard_interval,
	const std::string &length_tag_key,
	const std::string &trigger_tag_key,
	bool output_symbols,
	size_t itemsize)
    {
      return gnuradio::get_initial_sptr (
	  new header_payload_demux_impl(
	    header_len,
	    items_per_symbol,
	    guard_interval,
	    length_tag_key,
	    trigger_tag_key,
	    output_symbols,
	    itemsize
	  )
      );
    }

    header_payload_demux_impl::header_payload_demux_impl(
	int header_len,
	int items_per_symbol,
	int guard_interval,
	const std::string &length_tag_key,
	const std::string &trigger_tag_key,
	bool output_symbols,
	size_t itemsize
    ) : block("header_payload_demux",
		      io_signature::make2(1, 2, itemsize, sizeof(char)),
		      io_signature::make(2, 2, (output_symbols ? itemsize * items_per_symbol : itemsize))),
      d_header_len(header_len),
      d_items_per_symbol(items_per_symbol),
      d_gi(guard_interval),
      d_len_tag_key(pmt::string_to_symbol(length_tag_key)),
      d_trigger_tag_key(pmt::string_to_symbol(trigger_tag_key)),
      d_output_symbols(output_symbols),
      d_itemsize(itemsize),
      d_uses_trigger_tag(!trigger_tag_key.empty()),
      d_state(STATE_FIND_TRIGGER),
      d_payload_tag_keys(0),
      d_payload_tag_values(0)
    {
      if (d_header_len < 1) {
	throw std::invalid_argument("Header length must be at least 1 symbol.");
      }
      if (d_items_per_symbol < 1 || d_gi < 0 || d_itemsize < 1) {
	throw std::invalid_argument("Items and symbol sizes must be at least 1.");
      }
      if (!d_output_symbols) {
	set_output_multiple(d_items_per_symbol);
      }
      message_port_register_in(msg_port_id);
      set_msg_handler(msg_port_id, boost::bind(&header_payload_demux_impl::parse_header_data_msg, this, _1));
    }

    header_payload_demux_impl::~header_payload_demux_impl()
    {
    }

    void
    header_payload_demux_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      int n_items_reqd = 0;
      if (d_state == STATE_HEADER) {
	n_items_reqd = d_header_len * (d_items_per_symbol + d_gi);
      //} else if (d_state == STATE_HEADER) {
      } else {
	n_items_reqd = noutput_items * (d_items_per_symbol + d_gi);
	if (!d_output_symbols) {
	  // here, noutput_items is an integer multiple of d_items_per_symbol!
	  n_items_reqd /= d_items_per_symbol;
	}
      }

      for (unsigned i = 0; i < ninput_items_required.size(); i++) {
	ninput_items_required[i] = n_items_reqd;
      }
    }

    int
    header_payload_demux_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out_header = (unsigned char *) output_items[0];
      unsigned char *out_payload = (unsigned char *) output_items[1];

      int nread = 0;
      bool exit_loop = false;
      int produced_hdr = 0;
      int produced_payload = 0;

      while (
	  nread < noutput_items
	  && nread < ninput_items[0]
	  && (ninput_items.size() == 1 || nread < ninput_items[1])
	  && !exit_loop
      ) {
	switch (d_state) {
	  case STATE_WAIT_FOR_MSG:
	    // In an ideal world, this would never be called
	    return 0;

	  case STATE_HEADER_RX_FAIL:
	    // TODO: Consume one item from input when copy_symbols has been optimized
	    d_state = STATE_FIND_TRIGGER;

	  case STATE_FIND_TRIGGER:
	    // 1) Search for a trigger signal on input 1 (if present)
	    // 2) Search for a trigger tag, make sure it's the first one
	    // The first trigger to be found is used!
	    // 3) Make sure the right number of items is skipped
	    // 4) If trigger found, switch to STATE_HEADER
	    if (find_trigger_signal(nread, noutput_items, input_items)) {
	      d_remaining_symbols = d_header_len;
	      d_state = STATE_HEADER;
	      in += nread * d_itemsize;
	    }
	    break;

	  case STATE_HEADER:
	    copy_symbol(in, out_header, 0, nread, produced_hdr);
	    if (d_remaining_symbols == 0) {
	      d_state = STATE_WAIT_FOR_MSG;
	      exit_loop = true;
	    }
	    break;

	  case STATE_HEADER_RX_SUCCESS:
	    // TODO: Consume the entire header from the input when copy_symbols has been optimized
	    for (unsigned i = 0; i < d_payload_tag_keys.size(); i++) {
	      add_item_tag(1, nitems_written(1), d_payload_tag_keys[i], d_payload_tag_values[i]);
	    }
	    d_state = STATE_PAYLOAD;

	  case STATE_PAYLOAD:
	    copy_symbol(in, out_payload, 1, nread, produced_payload);
	    if (d_remaining_symbols == 0) {
	      d_state = STATE_FIND_TRIGGER;
	      exit_loop = true;
	    }
	    break;

	  default:
	    throw std::runtime_error("invalid state");
	} /* switch */
      } /* while(nread < noutput_items) */

      if (!d_output_symbols) {
	produced_hdr *= d_items_per_symbol;
	produced_payload *= d_items_per_symbol;
      }
      produce(0, produced_hdr);
      produce(1, produced_payload);
      consume_each (nread);
      return WORK_CALLED_PRODUCE;
    } /* general_work() */


    bool
    header_payload_demux_impl::find_trigger_signal(
	int &pos,
	int noutput_items,
	gr_vector_const_void_star &input_items)
    {
      if (input_items.size() == 2) {
	unsigned char *in_trigger = (unsigned char *) input_items[1];
	for (int i = 0; i < noutput_items; i++) {
	  if (in_trigger[i]) {
	    pos = i;
	    return true;
	  }
	}
      }
      if (d_uses_trigger_tag) {
	std::vector<tag_t> tags;
	get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0)+noutput_items);
	uint64_t min_offset = ULLONG_MAX;
	int tag_index = -1;
	for (unsigned i = 0; i < tags.size(); i++) {
	  if (tags[i].key == d_trigger_tag_key && tags[i].offset < min_offset) {
	    tag_index = (int) i;
	    min_offset = tags[i].offset;
	  }
	}
	if (tag_index != -1) {
	  pos = min_offset - nitems_read(0);
	  return true;
	}
      }
      pos += noutput_items;
      return false;
    }


    void
    header_payload_demux_impl::parse_header_data_msg(pmt::pmt_t header_data)
    {
      d_payload_tag_keys.clear();
      d_payload_tag_values.clear();
      d_state = STATE_HEADER_RX_FAIL;

      if (pmt::is_integer(header_data)) {
	d_remaining_symbols = pmt::to_long(header_data);
	d_payload_tag_keys.push_back(d_len_tag_key);
	d_payload_tag_values.push_back(header_data);
	d_state = STATE_HEADER_RX_SUCCESS;
      } else if (pmt::is_dict(header_data)) {
	pmt::pmt_t dict_items(pmt::dict_items(header_data));
	while (!pmt::is_null(dict_items)) {
	  pmt::pmt_t this_item(pmt::car(dict_items));
	  d_payload_tag_keys.push_back(pmt::car(this_item));
	  d_payload_tag_values.push_back(pmt::cdr(this_item));
	  if (pmt::equal(pmt::car(this_item), d_len_tag_key)) {
	    d_remaining_symbols = pmt::to_long(pmt::cdr(this_item));
	    d_state = STATE_HEADER_RX_SUCCESS;
	  }
	  dict_items = pmt::cdr(dict_items);
	}
	if (d_state == STATE_HEADER_RX_FAIL) {
	  GR_LOG_CRIT(d_logger, "no length tag passed from header data");
	}
      } else if (header_data == pmt::PMT_F || pmt::is_null(header_data)) {
	GR_LOG_INFO(d_logger, boost::format("Parser returned %1%") % pmt::write_string(header_data));
      } else {
	GR_LOG_ALERT(d_logger, boost::format("Received illegal header data (%1%)") % pmt::write_string(header_data));
      }
      if (d_state == STATE_HEADER_RX_SUCCESS
	  && (d_remaining_symbols * (d_output_symbols ? 1 : d_items_per_symbol)) > max_output_buffer(1)
      ) {
	d_state = STATE_HEADER_RX_FAIL;
	GR_LOG_INFO(d_logger, boost::format("Detected a packet larger than max frame size (%1% symbols)") % d_remaining_symbols);
      }
    }

    // This is a inefficient design: Can only copy one symbol at once. TODO fix.
    void
    header_payload_demux_impl::copy_symbol(const unsigned char *&in, unsigned char *&out, int port, int &nread, int &nproduced)
    {
      std::vector<tag_t> tags;
      memcpy((void *) out,
	     (void *) (in + d_gi * d_itemsize),
	     d_itemsize * d_items_per_symbol
      );
      // Tags on GI
      get_tags_in_range(tags, 0,
	                nitems_read(0) + nread,
			nitems_read(0) + nread + d_gi
      );
      for (unsigned t = 0; t < tags.size(); t++) {
	add_item_tag(port,
	    nitems_written(port)+nproduced,
	    tags[t].key,
	    tags[t].value
	);
      }
      // Tags on symbol
      get_tags_in_range(
	  tags, 0,
	  nitems_read(port) + nread + d_gi,
	  nitems_read(port) + nread + d_gi + d_items_per_symbol
      );
      for (unsigned t = 0; t < tags.size(); t++) {
	add_item_tag(0,
	    tags[t].offset - nitems_read(0)-nread + nitems_written(port)+nproduced,
	    tags[t].key,
	    tags[t].value
	);
      }
      in += d_itemsize * (d_items_per_symbol + d_gi);
      out += d_items_per_symbol * d_itemsize;
      nread += d_items_per_symbol + d_gi;
      nproduced++;
      d_remaining_symbols--;
    }

  } /* namespace digital */
} /* namespace gr */

