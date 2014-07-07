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

#include "message_burst_source_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <string.h>
#include <gnuradio/tags.h>

namespace gr {
  namespace blocks {

    message_burst_source::sptr
    message_burst_source::make(size_t itemsize, int msgq_limit)
    {
      return gnuradio::get_initial_sptr
        (new message_burst_source_impl(itemsize, msgq_limit));
    }

    message_burst_source::sptr
    message_burst_source::make(size_t itemsize, msg_queue::sptr msgq)
    {
      return gnuradio::get_initial_sptr
        (new message_burst_source_impl(itemsize, msgq));
    }

    message_burst_source_impl::message_burst_source_impl(size_t itemsize, int msgq_limit)
      : sync_block("message_burst_source",
                      io_signature::make(0, 0, 0),
                      io_signature::make(1, 1, itemsize)),
        d_itemsize(itemsize), d_msgq(msg_queue::make(msgq_limit)),
        d_msg_offset(0), d_eof(false)
    {
      std::stringstream id;
      id << name() << unique_id();
      d_me = pmt::string_to_symbol(id.str());
    }

    message_burst_source_impl::message_burst_source_impl(size_t itemsize, msg_queue::sptr msgq)
      : sync_block("message_burst_source",
                      io_signature::make(0, 0, 0),
                      io_signature::make(1, 1, itemsize)),
        d_itemsize(itemsize), d_msgq(msgq),
        d_msg_offset(0), d_eof(false)
    {
      std::stringstream id;
      id << name() << unique_id();
      d_me = pmt::string_to_symbol(id.str());
    }

    message_burst_source_impl::~message_burst_source_impl()
    {
    }

    int
    message_burst_source_impl::work(int noutput_items,
                                    gr_vector_const_void_star &input_items,
                                    gr_vector_void_star &output_items)
    {
      char *out = (char*)output_items[0];
      int nn = 0;

      uint64_t abs_sample_count = nitems_written(0);

      while(nn < noutput_items) {
        if(d_msg) {
	//
	// Consume whatever we can from the current message
	//

          int mm = std::min(noutput_items - nn,
                            (int)((d_msg->length() - d_msg_offset) / d_itemsize));
          memcpy(out, &(d_msg->msg()[d_msg_offset]), mm * d_itemsize);

          nn += mm;
          out += mm * d_itemsize;
          d_msg_offset += mm * d_itemsize;
          assert(d_msg_offset <= d_msg->length());

          if(d_msg_offset == d_msg->length()) {
	    if(d_msg->type() == 1)	           // type == 1 sets EOF
              d_eof = true;
	    d_msg.reset();
	    //tag end of burst
	    add_item_tag(0, //stream ID
			 abs_sample_count+nn-1, //sample number
			 pmt::string_to_symbol("tx_eob"),
			 pmt::from_bool(1),
			 d_me);        //block src id
          }
        }
        else {
          //
          // No current message
          //
          if(d_msgq->empty_p() && nn > 0) {    // no more messages in the queue, return what we've got
            break;
          }

          if(d_eof)
            return -1;

          d_msg = d_msgq->delete_head();	   // block, waiting for a message
          d_msg_offset = 0;
          //tag start of burst
          add_item_tag(0, //stream ID
                       abs_sample_count+nn, //sample number
                       pmt::string_to_symbol("tx_sob"),
                       pmt::from_bool(1),
                       d_me);        //block src id

          if((d_msg->length() % d_itemsize) != 0)
            throw std::runtime_error("msg length is not a multiple of d_itemsize");
        }
      }

      return nn;
    }

  } /* namespace blocks */
} /* namespace gr */
