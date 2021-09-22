/* -*- c++ -*- */
/*
 * Copyright 2013,2021 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/gr_complex.h>
#include <gnuradio/pdu.h>
#include <pmt/pmt.h>

namespace gr {
namespace metadata_keys {

const pmt::pmt_t pdu_num()
{
    static const pmt::pmt_t val = pmt::mp("pdu_num");
    return val;
}
const pmt::pmt_t rx_time()
{
    static const pmt::pmt_t val = pmt::mp("rx_time");
    return val;
}
const pmt::pmt_t sample_rate()
{
    static const pmt::pmt_t val = pmt::mp("sample_rate");
    return val;
}
const pmt::pmt_t sys_time()
{
    static const pmt::pmt_t val = pmt::mp("sys_time");
    return val;
}
const pmt::pmt_t tx_eob()
{
    static const pmt::pmt_t val = pmt::mp("tx_eob");
    return val;
}
const pmt::pmt_t tx_time()
{
    static const pmt::pmt_t val = pmt::mp("tx_time");
    return val;
}
const pmt::pmt_t tx_sob()
{
    static const pmt::pmt_t val = pmt::mp("tx_sob");
    return val;
}

} /* namespace metadata_keys */

namespace msgport_names {

const pmt::pmt_t bpdu()
{
    static const pmt::pmt_t val = pmt::mp("bpdu");
    return val;
}
const pmt::pmt_t conf()
{
    static const pmt::pmt_t val = pmt::mp("conf");
    return val;
}
const pmt::pmt_t cpdu()
{
    static const pmt::pmt_t val = pmt::mp("cpdu");
    return val;
}
const pmt::pmt_t detects()
{
    static const pmt::pmt_t val = pmt::mp("detects");
    return val;
}
const pmt::pmt_t dict()
{
    static const pmt::pmt_t val = pmt::mp("dict");
    return val;
}
const pmt::pmt_t fpdu()
{
    static const pmt::pmt_t val = pmt::mp("fpdu");
    return val;
}
const pmt::pmt_t msg()
{
    static const pmt::pmt_t val = pmt::mp("msg");
    return val;
}
const pmt::pmt_t pdu()
{
    static const pmt::pmt_t val = pmt::mp("pdu");
    return val;
}
const pmt::pmt_t pdus()
{
    static const pmt::pmt_t val = pmt::mp("pdus");
    return val;
}
const pmt::pmt_t vec()
{
    static const pmt::pmt_t val = pmt::mp("vec");
    return val;
}

} /* namespace msgport_names */

namespace pdu {

size_t itemsize(types::vector_type type)
{
    switch (type) {
    case types::byte_t:
        return sizeof(char);
    case types::short_t:
        return sizeof(short);
    case types::int_t:
        return sizeof(int);
    case types::float_t:
        return sizeof(float);
    case types::complex_t:
        return sizeof(gr_complex);
    default:
        throw std::runtime_error("bad PDU type");
    }
}

bool type_matches(types::vector_type type, pmt::pmt_t v)
{
    switch (type) {
    case types::byte_t:
        return pmt::is_u8vector(v);
    case types::short_t:
        return pmt::is_s16vector(v);
    case types::int_t:
        return pmt::is_s32vector(v);
    case types::float_t:
        return pmt::is_f32vector(v);
    case types::complex_t:
        return pmt::is_c32vector(v);
    default:
        throw std::runtime_error("bad PDU type");
    }
}

pmt::pmt_t make_pdu_vector(types::vector_type type, const uint8_t* buf, size_t items)
{
    switch (type) {
    case types::byte_t:
        return pmt::init_u8vector(items, buf);
    case types::short_t:
        return pmt::init_s16vector(items, (const short*)buf);
    case types::int_t:
        return pmt::init_s32vector(items, (const int*)buf);
    case types::float_t:
        return pmt::init_f32vector(items, (const float*)buf);
    case types::complex_t:
        return pmt::init_c32vector(items, (const gr_complex*)buf);
    default:
        throw std::runtime_error("bad PDU type");
    }
}

types::vector_type type_from_pmt(pmt::pmt_t vector)
{
    if (pmt::is_u8vector(vector))
        return types::byte_t;
    if (pmt::is_s16vector(vector))
        return types::short_t;
    if (pmt::is_s32vector(vector))
        return types::int_t;
    if (pmt::is_f32vector(vector))
        return types::float_t;
    if (pmt::is_c32vector(vector))
        return types::complex_t;
    throw std::runtime_error("bad PDU type");
}

} /* namespace pdu */
} /* namespace gr */
