/* -*- c++ -*- */
/*
 * Copyright 2021 NTESS LLC.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */


#ifndef INCLUDED_PDU_PORT_NAMES_H
#define INCLUDED_PDU_PORT_NAMES_H

#include <gnuradio/pdu/api.h>
#include <pmt/pmt.h>

namespace gr {
namespace pdu {


// static const PMT interned string getters
PDU_API const pmt::pmt_t PMTCONSTSTR__data();
PDU_API const pmt::pmt_t PMTCONSTSTR__dict();
PDU_API const pmt::pmt_t PMTCONSTSTR__emit();
PDU_API const pmt::pmt_t PMTCONSTSTR__msg();
PDU_API const pmt::pmt_t PMTCONSTSTR__pdus();


} // namespace pdu
} // namespace gr

#endif /* INCLUDED_PDU_PORT_NAMES_H */
