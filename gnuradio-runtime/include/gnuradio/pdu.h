/* -*- c++ -*- */
/*
 * Copyright 2013,2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_GR_PDU_H
#define INCLUDED_GR_PDU_H

#include <gnuradio/api.h>
#include <pmt/pmt.h>

namespace gr {
namespace metadata_keys {
GR_RUNTIME_API const pmt::pmt_t pdu_num();
GR_RUNTIME_API const pmt::pmt_t rx_time();
GR_RUNTIME_API const pmt::pmt_t sample_rate();
GR_RUNTIME_API const pmt::pmt_t sys_time();
GR_RUNTIME_API const pmt::pmt_t tx_eob();
GR_RUNTIME_API const pmt::pmt_t tx_time();
GR_RUNTIME_API const pmt::pmt_t tx_sob();
} /* namespace metadata_keys */

namespace msgport_names {
// static const PMT interned string getters for standard port names
GR_RUNTIME_API const pmt::pmt_t bpdu();
GR_RUNTIME_API const pmt::pmt_t conf();
GR_RUNTIME_API const pmt::pmt_t cpdu();
GR_RUNTIME_API const pmt::pmt_t detects();
GR_RUNTIME_API const pmt::pmt_t dict();
GR_RUNTIME_API const pmt::pmt_t fpdu();
GR_RUNTIME_API const pmt::pmt_t msg();
GR_RUNTIME_API const pmt::pmt_t pdu();
GR_RUNTIME_API const pmt::pmt_t pdus();
GR_RUNTIME_API const pmt::pmt_t vec();
} /* namespace msgport_names */

namespace types {
enum vector_type { byte_t, short_t, int_t, float_t, complex_t };
} /* namespace types */

namespace pdu {
// pdu functions
GR_RUNTIME_API size_t itemsize(types::vector_type type);
GR_RUNTIME_API bool type_matches(types::vector_type type, pmt::pmt_t v);
GR_RUNTIME_API pmt::pmt_t
make_pdu_vector(types::vector_type type, const uint8_t* buf, size_t items);
GR_RUNTIME_API types::vector_type type_from_pmt(pmt::pmt_t vector);

} // namespace pdu
} // namespace gr

#endif /* INCLUDED_GR_PDU_H */
