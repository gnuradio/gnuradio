/* -*- c++ -*- */
/*
 * Copyright 2013,2021 Free Software Foundation, Inc.
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


size_t itemsize(vector_type type)
{
    switch (type) {
    case byte_t:
        return sizeof(char);
    case float_t:
        return sizeof(float);
    case complex_t:
        return sizeof(gr_complex);
    default:
        throw std::runtime_error("bad PDU type");
    }
}

bool type_matches(vector_type type, pmt::pmt_t v)
{
    switch (type) {
    case byte_t:
        return pmt::is_u8vector(v);
    case float_t:
        return pmt::is_f32vector(v);
    case complex_t:
        return pmt::is_c32vector(v);
    default:
        throw std::runtime_error("bad PDU type");
    }
}

pmt::pmt_t make_pdu_vector(vector_type type, const uint8_t* buf, size_t items)
{
    switch (type) {
    case byte_t:
        return pmt::init_u8vector(items, buf);
    case float_t:
        return pmt::init_f32vector(items, (const float*)buf);
    case complex_t:
        return pmt::init_c32vector(items, (const gr_complex*)buf);
    default:
        throw std::runtime_error("bad PDU type");
    }
}

vector_type type_from_pmt(pmt::pmt_t vector)
{
    if (pmt::is_u8vector(vector))
        return byte_t;
    if (pmt::is_f32vector(vector))
        return float_t;
    if (pmt::is_c32vector(vector))
        return complex_t;
    throw std::runtime_error("bad PDU type");
}

} /* namespace pdu */
} /* namespace gr */
