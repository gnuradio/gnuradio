/* -*- c++ -*- */
/*
 * Copyright 2020 NTESS, LLC
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */


#ifndef INCLUDED_MESSAGE_PORT_NAMES_H
#define INCLUDED_MESSAGE_PORT_NAMES_H

#include <gnuradio/message/api.h>
#include <pmt/pmt.h>

namespace gr {
namespace message {


// static const PMT interned string getters
MESSAGE_API const pmt::pmt_t PMTCONSTSTR__emit();
MESSAGE_API const pmt::pmt_t PMTCONSTSTR__msg();


} // namespace message
} // namespace gr

#endif /* INCLUDED_MESSAGE_PORT_NAMES_H */
