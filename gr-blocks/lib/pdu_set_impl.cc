/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#include "pdu_set_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/blocks/pdu.h>

namespace gr {
  namespace blocks {

    pdu_set::sptr
    pdu_set::make(pmt::pmt_t k, pmt::pmt_t v)
    {
      return gnuradio::get_initial_sptr(new pdu_set_impl(k,v));
    }

    pdu_set_impl::pdu_set_impl(pmt::pmt_t k, pmt::pmt_t v)
      :	block("pdu_set",
		 io_signature::make (0, 0, 0),
		 io_signature::make (0, 0, 0)),
        d_k(k), d_v(v)
    {
      message_port_register_out(pdu::pdu_port_id());
      message_port_register_in(pdu::pdu_port_id());
      set_msg_handler(pdu::pdu_port_id(), boost::bind(&pdu_set_impl::handle_msg, this, _1));
    }

    void
    pdu_set_impl::handle_msg(pmt::pmt_t pdu)
    {
      // add the field and publish
      pmt::pmt_t meta = pmt::car(pdu);
      if(pmt::is_null(meta)){
        meta = pmt::make_dict();
        } else if(!pmt::is_dict(meta)){
        throw std::runtime_error("pdu_set received non PDU input");
        }
      meta = pmt::dict_add(meta, d_k, d_v);
      message_port_pub(pdu::pdu_port_id(), pmt::cons(meta, pmt::cdr(pdu)));
    }

  } /* namespace blocks */
}/* namespace gr */
