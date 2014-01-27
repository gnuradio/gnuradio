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

    //! Returns a PMT time tuple (uint seconds, double fraction) as the sum of
    //  another PMT time tuple and a time diff in seconds.
    pmt::pmt_t _update_pmt_time(
      pmt::pmt_t old_time,
      double time_difference
    ){
      double diff_seconds;
      double diff_frac = modf(time_difference, &diff_seconds);
      uint64_t seconds = pmt::to_uint64(pmt::tuple_ref(old_time, 0)) + (uint64_t) diff_seconds;
      double   frac    = pmt::to_double(pmt::tuple_ref(old_time, 1)) + diff_frac;
      return pmt::make_tuple(pmt::from_uint64(seconds), pmt::from_double(frac));
    }

    enum demux_states_t {
      STATE_FIND_TRIGGER,       // "Idle" state (waiting for burst)
      STATE_HEADER,             // Copy header
      STATE_WAIT_FOR_MSG,       // Null state (wait until msg from header demod)
      STATE_HEADER_RX_SUCCESS,  // Header processing
      STATE_HEADER_RX_FAIL,     //       "
      STATE_PAYLOAD             // Copy payload
    };

    enum out_port_indexes_t {
      PORT_HEADER = 0,
      PORT_PAYLOAD = 1
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
	size_t itemsize,
	const std::string &timing_tag_key,
	const double samp_rate,
	const std::vector<std::string> &special_tags
    ){
      return gnuradio::get_initial_sptr (
	  new header_payload_demux_impl(
	    header_len,
	    items_per_symbol,
	    guard_interval,
	    length_tag_key,
	    trigger_tag_key,
	    output_symbols,
	    itemsize,
	    timing_tag_key,
	    samp_rate,
	    special_tags
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
	size_t itemsize,
	const std::string &timing_tag_key,
	const double samp_rate,
	const std::vector<std::string> &special_tags
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
      d_payload_tag_values(0),
      d_track_time(!timing_tag_key.empty()),
      d_timing_key(pmt::intern(timing_tag_key)),
      d_last_time_offset(0),
      d_last_time(pmt::make_tuple(pmt::from_uint64(0L), pmt::from_double(0.0))),
      d_sampling_time(1.0/samp_rate)
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
      for (unsigned i = 0; i < special_tags.size(); i++) {
	d_special_tags.push_back(pmt::string_to_symbol(special_tags[i]));
	d_special_tags_last_value.push_back(pmt::PMT_NIL);
      }
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
      unsigned char *out_header = (unsigned char *) output_items[PORT_HEADER];
      unsigned char *out_payload = (unsigned char *) output_items[PORT_PAYLOAD];

      int nread = 0;
      int trigger_offset = 0;

      switch (d_state) {
	case STATE_WAIT_FOR_MSG:
	  // In an ideal world, this would never be called
	  return 0;

	case STATE_HEADER_RX_FAIL:
	  update_special_tags(0, 1);
	  consume_each (1);
	  in += d_itemsize;
	  nread++;
	  d_state = STATE_FIND_TRIGGER;
	  // Fall through

	case STATE_FIND_TRIGGER:
	  trigger_offset = find_trigger_signal(nread, noutput_items, input_items);
	  if (trigger_offset == -1) {
	    update_special_tags(0, noutput_items - nread);
	    consume_each(noutput_items - nread);
	    break;
	  }
	  update_special_tags(0, trigger_offset);
	  consume_each (trigger_offset);
	  in += trigger_offset * d_itemsize;
	  d_state = STATE_HEADER;
	  // Fall through

	case STATE_HEADER:
	  if (check_items_available(d_header_len, ninput_items, noutput_items, nread)) {
	    copy_n_symbols(in, out_header, PORT_HEADER, d_header_len);
	    d_state = STATE_WAIT_FOR_MSG;
	    add_special_tags();
	    produce(
		PORT_HEADER,
		d_header_len * (d_output_symbols ? 1 : d_items_per_symbol)
	    );
	  }
	  break;

	case STATE_HEADER_RX_SUCCESS:
	  for (unsigned i = 0; i < d_payload_tag_keys.size(); i++) {
	    add_item_tag(
		PORT_PAYLOAD,
		nitems_written(PORT_PAYLOAD),
		d_payload_tag_keys[i],
		d_payload_tag_values[i]
	    );
	  }
	  nread += d_header_len * (d_items_per_symbol + d_gi);
	  update_special_tags(0, nread);
	  consume_each (nread);
	  in += nread * d_itemsize;
	  d_state = STATE_PAYLOAD;
	  // Fall through

	case STATE_PAYLOAD:
	  if (check_items_available(d_curr_payload_len, ninput_items, noutput_items, nread)) {
	    // The -1 because we won't consume the last item, it might hold the next trigger.
	    update_special_tags(0, (d_curr_payload_len - 1) * (d_items_per_symbol + d_gi));
	    copy_n_symbols(in, out_payload, PORT_PAYLOAD, d_curr_payload_len);
	    produce(PORT_PAYLOAD, d_curr_payload_len * (d_output_symbols ? 1 : d_items_per_symbol));
	    consume_each ((d_curr_payload_len - 1) * (d_items_per_symbol + d_gi)); // Same here
	    set_min_noutput_items(d_output_symbols ? 1 : (d_items_per_symbol + d_gi));
	    d_state = STATE_FIND_TRIGGER;
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
	get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0)+noutput_items, d_trigger_tag_key);
	uint64_t min_offset = ULLONG_MAX;
	int tag_index = -1;
	for (unsigned i = 0; i < tags.size(); i++) {
	  if (tags[i].offset < min_offset) {
	    tag_index = (int) i;
	    min_offset = tags[i].offset;
	  }
	}
	if (tag_index != -1) {
	  remove_item_tag(0, tags[tag_index]);
	  return min_offset - nitems_read(0);
	}
      }
      return -1;
    } /* find_trigger_signal() */


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
	if ((d_curr_payload_len * (d_output_symbols ? 1 : d_items_per_symbol)) > max_output_buffer(1)/2) {
	  d_state = STATE_HEADER_RX_FAIL;
	  GR_LOG_INFO(d_logger, boost::format("Detected a packet larger than max frame size (%1% symbols)") % d_curr_payload_len);
	} else {
	  set_min_noutput_items(d_curr_payload_len * (d_output_symbols ? 1 : d_items_per_symbol));
	}
      }
    } /* parse_header_data_msg() */


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
    } /* copy_n_symbols() */

    void
    header_payload_demux_impl::update_special_tags(
	int range_start,
	int range_end
    ){
      if (d_track_time) {
	std::vector<tag_t> tags;
	get_tags_in_range(tags, 0,
	    nitems_read(0) + range_start,
	    nitems_read(0) + range_end,
	    d_timing_key
	);
	for (unsigned t = 0; t < tags.size(); t++) {
	  if(tags[t].offset >= d_last_time_offset) {
	    d_last_time = tags[t].value;
	    d_last_time_offset = tags[t].offset;
	  }
	}
      }

      std::vector<tag_t> tags;
      for (unsigned i = 0; i < d_special_tags.size(); i++) {
	uint64_t offset = 0;
	// TODO figure out if it's better to get all tags at once instead of doing this for every tag individually
	get_tags_in_range(tags, 0,
	    nitems_read(0) + range_start,
	    nitems_read(0) + range_end,
	    d_special_tags[i]
	);
	for (unsigned t = 0; t < tags.size(); t++) {
	  if(tags[t].offset >= offset) {
	    d_special_tags_last_value[i] = tags[t].value;
	    offset = tags[t].offset;
	  }
	}
      }
    } /* update_special_tags() */

    void
    header_payload_demux_impl::add_special_tags(
    ){
      if (d_track_time) {
	add_item_tag(
	    PORT_HEADER,
	    nitems_written(PORT_HEADER),
	    d_timing_key,
	    _update_pmt_time(
	      d_last_time,
	      d_sampling_time * (nitems_read(0) - d_last_time_offset)
	    )
	);
      }

      for (unsigned i = 0; i < d_special_tags.size(); i++) {
	add_item_tag(
	    PORT_HEADER,
	    nitems_written(PORT_HEADER),
	    d_special_tags[i],
	    d_special_tags_last_value[i]
	);
      }
    } /* add_special_tags() */

  } /* namespace digital */
} /* namespace gr */

