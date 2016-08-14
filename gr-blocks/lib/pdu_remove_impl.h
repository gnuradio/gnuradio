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

#ifndef INCLUDED_BLOCKS_PDU_REMOVE_IMPL_H
#define INCLUDED_BLOCKS_PDU_REMOVE_IMPL_H

#include <gnuradio/blocks/pdu_remove.h>

namespace gr {
  namespace blocks {

    class pdu_remove_impl : public pdu_remove
    {
    private:
      pmt::pmt_t d_k;

    public:
      pdu_remove_impl(pmt::pmt_t k);
      void handle_msg(pmt::pmt_t msg);
      void set_key(pmt::pmt_t key) { d_k = key; };
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_PDU_REMOVE_IMPL_H */
