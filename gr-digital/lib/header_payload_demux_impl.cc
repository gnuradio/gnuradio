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
      d_curr_payload_len(0),
      d_payload_tag_keys(0),
      d_payload_tag_values(0)
    {
      if (d_header_len < 1) {
	throw std::invalid_argument("Header length must be at least 1 symbol.");
      }
      if (d_items_per_symbol < 1 || d_gi < 0 || d_itemsize < 1) {
	throw std::invalid_argument("Items and symbol sizes must be at least 1.");
      }
      if (d_output_symbols) {
	set_relative_rate(1.0 / (d_items_per_symbol + d_gi));
      } else {
	set_relative_rate((double)d_items_per_symbol / (d_items_per_symbol + d_gi));
	set_output_multiple(d_items_per_symbol);
      }
      set_tag_propagation_policy(TPP_DONT);
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
      } else if (d_state == STATE_PAYLOAD) {
	n_items_reqd = d_curr_payload_len * (d_items_per_symbol + d_gi);
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


    inline bool
    header_payload_demux_impl::check_items_available(
	int n_symbols,
	gr_vector_int &ninput_items,
	int noutput_items,
	int nread
    )
    {
      // Check there's enough items on the input
      if ((n_symbols * (d_items_per_symbol + d_gi)) > (ninput_items[0]-nread)
	  || (ninput_items.size() == 2 && ((n_symbols * (d_items_per_symbol + d_gi)) > (ninput_items[1]-nread)))) {
	return false;
      }

      // Check there's enough space on the output buffer
      if (d_output_symbols) {
	if (noutput_items < n_symbols) {
	  return false;
	}
      } else {
	if (noutput_items < n_symbols * d_items_per_symbol) {
	  return false;
	}
      }

      return true;
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
      int trigger_offset = 0;

      switch (d_state) {
	case STATE_WAIT_FOR_MSG:
	  // In an ideal world, this would never be called
	  return 0;

	case STATE_HEADER_RX_FAIL:
	  consume_each (1);
	  in += d_itemsize;
	  nread++;
	  d_state = STATE_FIND_TRIGGER;

	case STATE_FIND_TRIGGER:
	  trigger_offset = find_trigger_signal(nread, noutput_items, input_items);
	  if (trigger_offset == -1) {
	    consume_each(noutput_items - nread);
	    break;
	  }
	  consume_each (trigger_offset);
	  in += trigger_offset * d_itemsize;
	  d_state = STATE_HEADER;


	case STATE_HEADER:
	  if (check_items_available(d_header_len, ninput_items, noutput_items, nread)) {
	    copy_n_symbols(in, out_header, 0, d_header_len);
	    d_state = STATE_WAIT_FOR_MSG;
	    produce(0, d_header_len * (d_output_symbols ? 1 : d_items_per_symbol));
	  }
	  break;

	case STATE_HEADER_RX_SUCCESS:
	  for (unsigned i = 0; i < d_payload_tag_keys.size(); i++) {
	    add_item_tag(1, nitems_written(1), d_payload_tag_keys[i], d_payload_tag_values[i]);
	  }
	  consume_each (d_header_len * (d_items_per_symbol + d_gi));
	  in += d_header_len * (d_items_per_symbol + d_gi) * d_itemsize;
	  nread += d_header_len * (d_items_per_symbol + d_gi);
	  d_state = STATE_PAYLOAD;

	case STATE_PAYLOAD:
	  if (check_items_available(d_curr_payload_len, ninput_items, noutput_items, nread)) {
	    copy_n_symbols(in, out_payload, 1, d_curr_payload_len);
	    produce(1, d_curr_payload_len * (d_output_symbols ? 1 : d_items_per_symbol));
	    consume_each (d_curr_payload_len * (d_items_per_symbol + d_gi));
	    d_state = STATE_FIND_TRIGGER;
	    set_min_noutput_items(d_output_symbols ? 1 : (d_items_per_symbol + d_gi));
	  }
	  break;

	default:
	  throw std::runtime_error("invalid state");
      } /* switch */

      return WORK_CALLED_PRODUCE;
    } /* general_work() */


    int
    header_payload_demux_impl::find_trigger_signal(
	int nread,
	int noutput_items,
	gr_vector_const_void_star &input_items)
    {
      if (input_items.size() == 2) {
	unsigned char *in_trigger = (unsigned char *) input_items[1];
	in_trigger += nread;
	for (int i = 0; i < noutput_items-nread; i++) {
	  if (in_trigger[i]) {
	    return i;
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
	  return min_offset - nitems_read(0);
	}
      }
      return -1;
    }


    void
    header_payload_demux_impl::parse_header_data_msg(pmt::pmt_t header_data)
    {
      d_payload_tag_keys.clear();
      d_payload_tag_values.clear();
      d_state = STATE_HEADER_RX_FAIL;

      if (pmt::is_integer(header_data)) {
	d_curr_payload_len = pmt::to_long(header_data);
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
	    d_curr_payload_len = pmt::to_long(pmt::cdr(this_item));
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
      if (d_state == STATE_HEADER_RX_SUCCESS)
      {
	if ((d_curr_payload_len * (d_output_symbols ? 1 : d_items_per_symbol)) > max_output_buffer(1)) {
	  d_state = STATE_HEADER_RX_FAIL;
	  GR_LOG_INFO(d_logger, boost::format("Detected a packet larger than max frame size (%1% symbols)") % d_curr_payload_len);
	} else {
	  set_min_noutput_items(d_curr_payload_len * (d_output_symbols ? 1 : d_items_per_symbol));
	}
      }
    }


    void
    header_payload_demux_impl::copy_n_symbols(
	const unsigned char *in,
	unsigned char *out,
	int port,
	int n_symbols
    )
    {
      // Copy samples
      if (d_gi) {
	for (int i = 0; i < n_symbols; i++) {
	  memcpy((void *) out, (void *) (in + d_gi * d_itemsize), d_items_per_symbol * d_itemsize);
	  in  += d_itemsize * (d_items_per_symbol + d_gi);
	  out += d_itemsize * d_items_per_symbol;
	}
      } else {
	memcpy(
	    (void *) out,
	    (void *) in,
	    n_symbols * d_items_per_symbol * d_itemsize
	);
      }
      // Copy tags
      std::vector<tag_t> tags;
      get_tags_in_range(
	  tags, 0,
	  nitems_read(0),
	  nitems_read(0) + n_symbols * (d_items_per_symbol + d_gi)
      );
      for (unsigned t = 0; t < tags.size(); t++) {
	int new_offset = tags[t].offset - nitems_read(0);
	if (d_output_symbols) {
	  new_offset /= (d_items_per_symbol + d_gi);
	} else if (d_gi) {
	  int pos_on_symbol = (new_offset % (d_items_per_symbol + d_gi)) - d_gi;
	  if (pos_on_symbol < 0) {
	    pos_on_symbol = 0;
	  }
	  new_offset = (new_offset / (d_items_per_symbol + d_gi)) + pos_on_symbol;
	}
	add_item_tag(port,
	    nitems_written(port) + new_offset,
	    tags[t].key,
	    tags[t].value
	);
      }
    }

  } /* namespace digital */
} /* namespace gr */

