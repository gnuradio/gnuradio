/* -*- c++ -*- */
/*
 * Copyright 2013,2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_PDU_PDU_H
#define INCLUDED_PDU_PDU_H

#include <gnuradio/gr_complex.h>
#include <gnuradio/pdu/api.h>
#include <pmt/pmt.h>

namespace gr {
namespace pdu {
enum vector_type { byte_t, float_t, complex_t };


// static const PMT interned string getters
PDU_API const pmt::pmt_t PMTCONSTSTR__data();
PDU_API const pmt::pmt_t PMTCONSTSTR__dict();
PDU_API const pmt::pmt_t PMTCONSTSTR__emit();
PDU_API const pmt::pmt_t PMTCONSTSTR__msg();
PDU_API const pmt::pmt_t PMTCONSTSTR__pdus();

// pdu functions
PDU_API size_t itemsize(vector_type type);
PDU_API bool type_matches(vector_type type, pmt::pmt_t v);
PDU_API pmt::pmt_t make_pdu_vector(vector_type type, const uint8_t* buf, size_t items);
PDU_API vector_type type_from_pmt(pmt::pmt_t vector);


} // namespace pdu
} // namespace gr

#endif /* INCLUDED_PDU_PDU_H */
