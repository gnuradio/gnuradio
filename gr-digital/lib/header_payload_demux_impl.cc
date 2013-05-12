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

#include <climits>
#include <gnuradio/io_signature.h>
#include "header_payload_demux_impl.h"

namespace gr {
  namespace digital {

    enum demux_states_t {
      STATE_IDLE,
      STATE_HEADER,
      STATE_WAIT_FOR_MSG,
      STATE_PAYLOAD
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
      d_state(STATE_IDLE)
    {
      if (d_header_len < 1) {
	throw std::invalid_argument("Header length must be at least 1 symbol.");
      }
      if (d_items_per_symbol < 1 || d_gi < 0 || d_itemsize < 1) {
	throw std::invalid_argument("Items and symbol sizes must be at least 1.");
      }
      set_output_multiple(d_items_per_symbol);
      message_port_register_in(msg_port_id);
    }

    header_payload_demux_impl::~header_payload_demux_impl()
    {
    }

    void
    header_payload_demux_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      // noutput_items is an integer multiple of d_items_per_symbol!
      for (unsigned i = 0; i < ninput_items_required.size(); i++) {
	ninput_items_required[i] =
	  noutput_items / d_items_per_symbol * (d_items_per_symbol + d_gi);
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

      // FIXME ninput_items[1] does not have to be defined O_o
      while (nread < noutput_items && nread < ninput_items[0] && nread < ninput_items[1] && !exit_loop) {
	switch (d_state) {
	      case STATE_IDLE:
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

	      case STATE_WAIT_FOR_MSG:
		// If we're in this state, nread is zero (because previous state exits loop)
		// 1) Wait for msg (blocking call)
		// 2) set d_remaining_symbols
		// 3) Write tags
		// 4) fall through to next state
		d_remaining_symbols = -1;
		if (!parse_header_data_msg()) {
		  d_state = STATE_IDLE;
		  exit_loop = true;
		  break;
		}
		d_state = STATE_PAYLOAD;

	      case STATE_PAYLOAD:
		copy_symbol(in, out_payload, 1, nread, produced_payload);
		if (d_remaining_symbols == 0) {
		  d_state = STATE_IDLE;
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


    bool
    header_payload_demux_impl::parse_header_data_msg()
    {
      pmt::pmt_t msg(delete_head_blocking(msg_port_id));
      if (pmt::is_integer(msg)) {
	d_remaining_symbols = pmt::to_long(msg);
	add_item_tag(1, nitems_written(1), d_len_tag_key, msg);
      } else if (pmt::is_dict(msg)) {
	pmt::pmt_t dict_items(pmt::dict_items(msg));
	while (!pmt::is_null(dict_items)) {
	  pmt::pmt_t this_item(pmt::car(dict_items));
	  add_item_tag(1, nitems_written(1), pmt::car(this_item), pmt::cdr(this_item));
	  if (pmt::equal(pmt::car(this_item), d_len_tag_key)) {
	    d_remaining_symbols = pmt::to_long(pmt::cdr(this_item));
	  }
	  dict_items = pmt::cdr(dict_items);
	}
	if (d_remaining_symbols == -1) {
	  throw std::runtime_error("no length tag passed from header data");
	}
      } else if (pmt::is_null(msg)) { // Blocking call was interrupted
	return false;
      } else if (msg == pmt::PMT_F) { // Header was invalid
	return false;
      } else {
	throw std::runtime_error("Received illegal header data");
      }
      return true;
    }

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

