/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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
