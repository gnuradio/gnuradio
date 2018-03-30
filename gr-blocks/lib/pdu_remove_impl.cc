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

#include "pdu_remove_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/blocks/pdu.h>

namespace gr {
  namespace blocks {

    pdu_remove::sptr
    pdu_remove::make(pmt::pmt_t k)
    {
      return gnuradio::get_initial_sptr(new pdu_remove_impl(k));
    }

    pdu_remove_impl::pdu_remove_impl(pmt::pmt_t k)
      :	block("pdu_remove",
		 io_signature::make (0, 0, 0),
		 io_signature::make (0, 0, 0)),
    d_k(k)
    {
      message_port_register_out(pdu::pdu_port_id());
      message_port_register_in(pdu::pdu_port_id());
      set_msg_handler(pdu::pdu_port_id(), boost::bind(&pdu_remove_impl::handle_msg, this, _1));
    }

    void
    pdu_remove_impl::handle_msg(pmt::pmt_t pdu)
    {
      // add the field and publish
      pmt::pmt_t meta = pmt::car(pdu);
      if(pmt::is_null(meta)){
        meta = pmt::make_dict();
        } else if(!pmt::is_dict(meta)){
        throw std::runtime_error("pdu_remove received non PDU input");
        }
      meta = pmt::dict_delete(meta, d_k);
      message_port_pub(pdu::pdu_port_id(), pmt::cons(meta, pmt::cdr(pdu)));
    }

  } /* namespace blocks */
}/* namespace gr */
