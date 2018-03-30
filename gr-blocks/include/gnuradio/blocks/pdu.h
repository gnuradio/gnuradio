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

#ifndef INCLUDED_BLOCKS_PDU_H
#define INCLUDED_BLOCKS_PDU_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/gr_complex.h>
#include <pmt/pmt.h>


namespace gr {
  namespace blocks {
    namespace pdu {
      enum vector_type { byte_t, float_t, complex_t };

      BLOCKS_API const pmt::pmt_t pdu_port_id();
      BLOCKS_API size_t itemsize(vector_type type);
      BLOCKS_API bool type_matches(vector_type type, pmt::pmt_t v);
      BLOCKS_API pmt::pmt_t make_pdu_vector(vector_type type, const uint8_t* buf, size_t items);
      BLOCKS_API vector_type type_from_pmt(pmt::pmt_t vector);

    } /* namespace pdu */
  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_PDU_H */
