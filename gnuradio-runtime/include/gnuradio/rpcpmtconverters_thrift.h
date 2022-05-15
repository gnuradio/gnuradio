/*
 * Copyright 2014,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef RPCPMTCONVERTERS_THRIFT_H
#define RPCPMTCONVERTERS_THRIFT_H

#include "thrift/gnuradio_types.h"
#include <pmt/pmt.h>
#include <map>


namespace rpcpmtconverter {
GNURadio::Knob from_pmt(const pmt::pmt_t& knob);

struct to_pmt_f {
    to_pmt_f() { ; }
    virtual ~to_pmt_f() {}
    virtual pmt::pmt_t operator()(const GNURadio::Knob& knob);
};

struct to_pmt_byte_f : public to_pmt_f {
    pmt::pmt_t operator()(const GNURadio::Knob& knob);
};
struct to_pmt_short_f : public to_pmt_f {
    pmt::pmt_t operator()(const GNURadio::Knob& knob);
};
struct to_pmt_int_f : public to_pmt_f {
    pmt::pmt_t operator()(const GNURadio::Knob& knob);
};
struct to_pmt_long_f : public to_pmt_f {
    pmt::pmt_t operator()(const GNURadio::Knob& knob);
};
struct to_pmt_double_f : public to_pmt_f {
    pmt::pmt_t operator()(const GNURadio::Knob& knob);
};
struct to_pmt_string_f : public to_pmt_f {
    pmt::pmt_t operator()(const GNURadio::Knob& knob);
};
struct to_pmt_bool_f : public to_pmt_f {
    pmt::pmt_t operator()(const GNURadio::Knob& knob);
};
struct to_pmt_complex_f : public to_pmt_f {
    pmt::pmt_t operator()(const GNURadio::Knob& knob);
};
struct to_pmt_f32vect_f : public to_pmt_f {
    pmt::pmt_t operator()(const GNURadio::Knob& knob);
};
struct to_pmt_f64vect_f : public to_pmt_f {
    pmt::pmt_t operator()(const GNURadio::Knob& knob);
};
struct to_pmt_s64vect_f : public to_pmt_f {
    pmt::pmt_t operator()(const GNURadio::Knob& knob);
};
struct to_pmt_s32vect_f : public to_pmt_f {
    pmt::pmt_t operator()(const GNURadio::Knob& knob);
};
struct to_pmt_s16vect_f : public to_pmt_f {
    pmt::pmt_t operator()(const GNURadio::Knob& knob);
};
struct to_pmt_s8vect_f : public to_pmt_f {
    pmt::pmt_t operator()(const GNURadio::Knob& knob);
};
struct to_pmt_c32vect_f : public to_pmt_f {
    pmt::pmt_t operator()(const GNURadio::Knob& knob);
};

class To_PMT
{
public:
    To_PMT(const To_PMT&) = delete;
    To_PMT& operator=(const To_PMT&) = delete;

    static To_PMT instance;
    template <typename TO_PMT_F>
    friend struct to_pmt_reg;
    pmt::pmt_t operator()(const GNURadio::Knob& knob);

protected:
    std::map<GNURadio::BaseTypes::type, to_pmt_f> to_pmt_map;

private:
    To_PMT() { ; }
};

template <typename TO_PMT_F>
struct to_pmt_reg {
    to_pmt_reg(To_PMT& instance, const GNURadio::BaseTypes::type type);
};
} // namespace rpcpmtconverter

#endif /* RPCPMTCONVERTERS_THRIFT_H */
