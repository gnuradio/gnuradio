/* -*- c++ -*- */
/* Copyright 2012-2016 Free Software Foundation, Inc.
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

#include "header_payload_demux_impl.h"
#include <gnuradio/io_signature.h>
#include <boost/format.hpp>
#include <climits>

namespace gr {
  namespace digital {

    const pmt::pmt_t header_payload_demux_impl::msg_port_id()
    {
      static const pmt::pmt_t msg_port_id = pmt::mp("header_data");
      return msg_port_id;
    }

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
      PORT_PAYLOAD = 1,
      PORT_INPUTDATA = 0,
      PORT_TRIGGER = 1
    };

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
        const std::vector<std::string> &special_tags,
        const size_t header_padding
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
            special_tags,
            header_padding
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
        const std::vector<std::string> &special_tags,
        const size_t header_padding
    ) : block("header_payload_demux",
          io_signature::make2(1, 2, itemsize, sizeof(char)),
          io_signature::make(2, 2, (output_symbols ? itemsize * items_per_symbol : itemsize))),
      d_header_len(header_len),
      d_header_padding_symbols(header_padding / items_per_symbol),
      d_header_padding_items(header_padding % items_per_symbol),
      d_header_padding_total_items(header_padding),
      d_items_per_symbol(items_per_symbol),
      d_gi(guard_interval),
      d_len_tag_key(pmt::string_to_symbol(length_tag_key)),
      d_trigger_tag_key(pmt::string_to_symbol(trigger_tag_key)),
      d_output_symbols(output_symbols),
      d_itemsize(itemsize),
      d_uses_trigger_tag(!trigger_tag_key.empty()),
      d_state(STATE_FIND_TRIGGER),
      d_curr_payload_len(0),
      d_curr_payload_offset(0),
      d_payload_tag_keys(0),
      d_payload_tag_values(0),
      d_track_time(!timing_tag_key.empty()),
      d_timing_key(pmt::intern(timing_tag_key)),
      d_payload_offset_key(pmt::intern("payload_offset")),
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
      if ((d_output_symbols || d_gi) && d_header_padding_items) {
        throw std::invalid_argument(
            "If output_symbols is true or a guard interval is given, padding must be a multiple of items_per_symbol!"
        );
      }
      set_tag_propagation_policy(TPP_DONT);
      message_port_register_in(msg_port_id());
      set_msg_handler(msg_port_id(), boost::bind(&header_payload_demux_impl::parse_header_data_msg, this, _1));
      for (size_t i = 0; i < special_tags.size(); i++) {
        d_special_tags.push_back(pmt::string_to_symbol(special_tags[i]));
        d_special_tags_last_value.push_back(pmt::PMT_NIL);
      }
    }

    header_payload_demux_impl::~header_payload_demux_impl()
    {
    }

    // forecast() depends on state:
    // - When waiting for a header, we require at least the header length
    // - when waiting for a payload, we require at least the payload length
    // - Otherwise, pretend this is a sync block with a decimation/interpolation
    //   depending on symbol size and if we output symbols or items
    void
    header_payload_demux_impl::forecast(
        int noutput_items,
        gr_vector_int &ninput_items_required
    ) {
      int n_items_reqd = 0;
      if (d_state == STATE_HEADER) {
        n_items_reqd = d_header_len * (d_items_per_symbol + d_gi)
                       + 2*d_header_padding_total_items;
      } else if (d_state == STATE_PAYLOAD) {
        n_items_reqd = d_curr_payload_len * (d_items_per_symbol + d_gi);
      } else {
        n_items_reqd = noutput_items * (d_items_per_symbol + d_gi);
        if (!d_output_symbols) {
          // Here, noutput_items is an integer multiple of d_items_per_symbol!
          n_items_reqd /= d_items_per_symbol;
        }
      }

      for (unsigned i = 0; i < ninput_items_required.size(); i++) {
        ninput_items_required[i] = n_items_reqd;
      }
    }


    bool header_payload_demux_impl::check_buffers_ready(
        int output_symbols_reqd,
        int extra_output_items_reqd,
        int noutput_items,
        int input_items_reqd,
        gr_vector_int &ninput_items,
        int n_items_read
    ) {
      // Check there's enough space on the output buffer
      if (d_output_symbols) {
        if (noutput_items < output_symbols_reqd + extra_output_items_reqd) {
          return false;
        }
      } else {
        if (noutput_items < (output_symbols_reqd * d_items_per_symbol) + extra_output_items_reqd) {
          return false;
        }
      }

      // Check there's enough items on the input
      if (input_items_reqd > (ninput_items[0]-n_items_read)
          || (ninput_items.size() == 2 && (input_items_reqd > (ninput_items[1]-n_items_read)))) {
        return false;
      }

      // All good
      return true;
    }


    int
    header_payload_demux_impl::general_work(
            int noutput_items,
            gr_vector_int &ninput_items,
            gr_vector_const_void_star &input_items,
            gr_vector_void_star &output_items
    ) {
      const unsigned char *in = (const unsigned char *) input_items[PORT_INPUTDATA];
      unsigned char *out_header = (unsigned char *) output_items[PORT_HEADER];
      unsigned char *out_payload = (unsigned char *) output_items[PORT_PAYLOAD];

      const int n_input_items = (ninput_items.size() == 2) ?
                                std::min(ninput_items[0], ninput_items[1]) :
                                ninput_items[0];
      // Items read going into general_work()
      const uint64_t n_items_read_base = nitems_read(PORT_INPUTDATA);
      // Items read during this call to general_work()
      int n_items_read = 0;

      #define CONSUME_ITEMS(items_to_consume) \
          update_special_tags( \
              n_items_read_base + n_items_read, \
              n_items_read_base + n_items_read + (items_to_consume) \
          ); \
          consume_each(items_to_consume); \
          n_items_read += (items_to_consume); \
          in += (items_to_consume) * d_itemsize;
      switch (d_state) {
        case STATE_WAIT_FOR_MSG:
          // In an ideal world, this would never be called
          // parse_header_data_msg() is the only place that can kick us out
          // of this state.
          return 0;

        case STATE_HEADER_RX_FAIL:
          // Actions:
          // - Consume a single item to make sure we're not deleting any other
          //   info
          CONSUME_ITEMS(1);
          d_state = STATE_FIND_TRIGGER;
          break;

        case STATE_FIND_TRIGGER: {
          // Assumptions going into this state:
          // - No other state was active for this call to general_work()
          //   - i.e. n_items_read == 0
          // Start looking for a trigger after any header padding.
          // The trigger offset is relative to 'in'.
          // => The absolute trigger offset is on n_items_read_base + n_items_read_base + trigger_offset
          const int max_rel_offset = n_input_items - n_items_read;
          const int trigger_offset = find_trigger_signal(
              d_header_padding_total_items,
              max_rel_offset,
              n_items_read_base + n_items_read,
              (input_items.size() == 2) ?
                  ((const unsigned char *) input_items[PORT_TRIGGER]) + n_items_read : NULL
          );
          if (trigger_offset < max_rel_offset) {
            d_state = STATE_HEADER;
          }
          // If we're using padding, don't consume everything, or we might
          // end up with not enough items before the trigger
          const int items_to_consume = trigger_offset - d_header_padding_total_items;
          CONSUME_ITEMS(items_to_consume);
          break;
        } /* case STATE_FIND_TRIGGER */

        case STATE_HEADER:
          // Assumptions going into this state:
          // - The first items on `in' are the header samples (including padding)
          //   - So we can just copy from the beginning of `in'
          // - The trigger is on item index `d_header_padding * d_items_per_symbol'
          // Actions:
          // - Copy the entire header (including padding) to the header port
          //   - Special tags are added to the header port
          if (check_buffers_ready(
                d_header_len + 2*d_header_padding_symbols,
                d_header_padding_items,
                noutput_items,
                d_header_len * (d_items_per_symbol + d_gi) + 2*d_header_padding_total_items,
                ninput_items,
                n_items_read)) {
            add_special_tags();
            copy_n_symbols(
                in,
                out_header,
                PORT_HEADER,
                n_items_read_base + n_items_read,
                d_header_len+2*d_header_padding_symbols, // Number of symbols to copy
                2*d_header_padding_items
            );
            d_state = STATE_WAIT_FOR_MSG;
          }
          break;

        case STATE_HEADER_RX_SUCCESS:
          // Copy tags from header to payload
          for (size_t i = 0; i < d_payload_tag_keys.size(); i++) {
            add_item_tag(
                PORT_PAYLOAD,
                nitems_written(PORT_PAYLOAD),
                d_payload_tag_keys[i],
                d_payload_tag_values[i]
            );
          }
          // Consume header from input
          {
            // Consume the padding only once, we leave the second
            // part in there because it might be part of the payload
            const int items_to_consume =
                            d_header_len * (d_items_per_symbol + d_gi)
                            + d_header_padding_total_items
                            + d_curr_payload_offset;
            CONSUME_ITEMS(items_to_consume);
            d_curr_payload_offset = 0;
            d_state = STATE_PAYLOAD;
          }
          break;

        case STATE_PAYLOAD:
          // Assumptions:
          // - Input buffer is in the right spot to just start copying
          if (check_buffers_ready(
                d_curr_payload_len,
                0,
                noutput_items,
                d_curr_payload_len * (d_items_per_symbol + d_gi),
                ninput_items,
                n_items_read)) {
            // Write payload
            copy_n_symbols(
                in,
                out_payload,
                PORT_PAYLOAD,
                n_items_read_base + n_items_read,
                d_curr_payload_len
            );
            // Consume payload
            // We can't consume the full payload, because we need to hold off
            // at least the padding value. We'll use a minimum padding of 1
            // item here.
            const int items_padding = std::max(d_header_padding_total_items, 1);
            const int items_to_consume =
                      d_curr_payload_len * (d_items_per_symbol + d_gi)
                      - items_padding;
            CONSUME_ITEMS(items_to_consume);
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
          int skip_items,
          int max_rel_offset,
          uint64_t base_offset,
          const unsigned char *in_trigger
    ) {
      int rel_offset = max_rel_offset;
      if (max_rel_offset < skip_items) {
        return rel_offset;
      }
      if (in_trigger) {
        for (int i = skip_items; i < max_rel_offset; i++) {
          if (in_trigger[i]) {
            rel_offset = i;
            break;
          }
        }
      }
      if (d_uses_trigger_tag) {
        std::vector<tag_t> tags;
        get_tags_in_range(
            tags,
            PORT_INPUTDATA,
            base_offset + skip_items,
            base_offset + max_rel_offset,
            d_trigger_tag_key
        );
        if (!tags.empty()) {
          std::sort(tags.begin(), tags.end(), tag_t::offset_compare);
          const int tag_rel_offset = tags[0].offset - base_offset;
          if (tag_rel_offset < rel_offset) {
            rel_offset = tag_rel_offset;
          }
        }
      }
      return rel_offset;
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
          if (pmt::equal(pmt::car(this_item), d_payload_offset_key)) {
            d_curr_payload_offset = pmt::to_long(pmt::cdr(this_item));
            if (std::abs(d_curr_payload_offset) > d_header_padding_total_items) {
              GR_LOG_CRIT(d_logger, "Payload offset exceeds padding");
              d_state = STATE_HEADER_RX_FAIL;
              return;
            }
          }
          dict_items = pmt::cdr(dict_items);
        }
        if (d_state == STATE_HEADER_RX_FAIL) {
          GR_LOG_CRIT(d_logger, "no payload length passed from header data");
        }
      } else if (header_data == pmt::PMT_F || pmt::is_null(header_data)) {
        GR_LOG_INFO(d_logger, boost::format("Parser returned %1%") % pmt::write_string(header_data));
      } else {
        GR_LOG_ALERT(d_logger, boost::format("Received illegal header data (%1%)") % pmt::write_string(header_data));
      }
      if (d_state == STATE_HEADER_RX_SUCCESS)
      {
        if (d_curr_payload_len < 0) {
          GR_LOG_WARN(d_logger, boost::format("Detected a packet larger than max frame size (%1% symbols)") % d_curr_payload_len);
          d_curr_payload_len = 0;
          d_state = STATE_HEADER_RX_FAIL;
        }
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
        const uint64_t n_items_read_base,
        int n_symbols,
        int n_padding_items
    ) {
      // Copy samples
      if (d_gi) {
        // Here we know n_padding_items must be 0 (see contract),
        // because all padding items will be part of n_symbols
        for (int i = 0; i < n_symbols; i++) {
          memcpy(
              (void *) out,
              (void *) (in + d_gi * d_itemsize),
              d_items_per_symbol * d_itemsize
          );
          in  += d_itemsize * (d_items_per_symbol + d_gi);
          out += d_itemsize * d_items_per_symbol;
        }
      } else {
        memcpy(
            (void *) out,
            (void *) in,
            (n_symbols * d_items_per_symbol + n_padding_items) * d_itemsize
        );
      }
      // Copy tags
      std::vector<tag_t> tags;
      get_tags_in_range(
          tags,
          PORT_INPUTDATA,
          n_items_read_base,
          n_items_read_base
            + n_symbols * (d_items_per_symbol + d_gi)
            + n_padding_items
      );
      for (size_t t = 0; t < tags.size(); t++) {
        // The trigger tag is *not* propagated
        if (tags[t].key == d_trigger_tag_key) {
          continue;
        }
        int new_offset = tags[t].offset - n_items_read_base;
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
      // Advance write pointers
      // Items to produce might actually be symbols
      const int items_to_produce = d_output_symbols ?
                            n_symbols :
                            (n_symbols * d_items_per_symbol + n_padding_items);
      produce(port, items_to_produce);
    } /* copy_n_symbols() */

    void
    header_payload_demux_impl::update_special_tags(
        uint64_t range_start,
        uint64_t range_end
    ){
      if (d_track_time) {
        std::vector<tag_t> tags;
        get_tags_in_range(
            tags,
            PORT_INPUTDATA,
            range_start,
            range_end,
            d_timing_key
        );
        if (!tags.empty()) {
          std::sort(tags.begin(), tags.end(), tag_t::offset_compare);
          d_last_time = tags.back().value;
          d_last_time_offset = tags.back().offset;
        }
      }

      std::vector<tag_t> tags;
      for (size_t i = 0; i < d_special_tags.size(); i++) {
        // TODO figure out if it's better to get all tags at once instead of doing this for every tag individually
        get_tags_in_range(
            tags,
            PORT_INPUTDATA, // Read from port 0
            range_start,
            range_end,
            d_special_tags[i]
        );
        std::sort(tags.begin(), tags.end(), tag_t::offset_compare);
        for (size_t t = 0; t < tags.size(); t++) {
          d_special_tags_last_value[i] = tags[t].value;
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
              d_sampling_time * (nitems_read(PORT_INPUTDATA) - d_last_time_offset)
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

