/* -*- c++ -*- */
/*
 * Copyright 2021 NTESS LLC.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/pdu/pdu.h>
#include <pmt/pmt.h>

namespace gr {
namespace pdu {


const pmt::pmt_t PMTCONSTSTR__data()
{
    static const pmt::pmt_t val = pmt::mp("data");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__dict()
{
    static const pmt::pmt_t val = pmt::mp("dict");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__emit()
{
    static const pmt::pmt_t val = pmt::mp("emit");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__msg()
{
    static const pmt::pmt_t val = pmt::mp("msg");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__pdus()
{
    static const pmt::pmt_t val = pmt::mp("pdus");
    return val;
}


} /* namespace pdu */
} /* namespace gr */
