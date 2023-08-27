/*
 * Copyright 2014,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "thrift/gnuradio_types.h"
#include <gnuradio/gr_complex.h>
#include <gnuradio/logger.h>
#include <gnuradio/rpcpmtconverters_thrift.h>

GNURadio::Knob rpcpmtconverter::from_pmt(const pmt::pmt_t& knob)
{
    if (pmt::is_real(knob)) {
        GNURadio::Knob result;
        result.type = GNURadio::BaseTypes::DOUBLE;
        result.value.__set_a_double(pmt::to_double(knob));
        return result;
    } else if (pmt::is_symbol(knob)) {
        std::string value = pmt::symbol_to_string(knob);
        GNURadio::Knob result;
        result.type = GNURadio::BaseTypes::STRING;
        result.value.__set_a_string(value);
        return result;
    } else if (pmt::is_integer(knob)) {
        GNURadio::Knob result;
        result.type = GNURadio::BaseTypes::LONG;
        result.value.__set_a_long(pmt::to_long(knob));
        return result;
    } else if (pmt::is_bool(knob)) {
        GNURadio::Knob result;
        result.type = GNURadio::BaseTypes::BOOL;
        result.value.__set_a_bool(pmt::to_bool(knob));
        return result;
    } else if (pmt::is_uint64(knob)) {
        GNURadio::Knob result;
        result.type = GNURadio::BaseTypes::LONG;
        result.value.__set_a_long(pmt::to_uint64(knob));
        return result;
    } else if (pmt::is_complex(knob)) {
        GNURadio::Knob result;
        result.type = GNURadio::BaseTypes::COMPLEX;
        std::complex<double> tmp = pmt::to_complex(knob);
        GNURadio::complex cpx;
        cpx.re = tmp.real();
        cpx.im = tmp.imag();
        result.value.__set_a_complex(cpx);
        return result;
    } else if (pmt::is_f32vector(knob)) {
        GNURadio::Knob result;
        result.type = GNURadio::BaseTypes::F32VECTOR;
        size_t size(pmt::length(knob));
        const float* start((const float*)pmt::f32vector_elements(knob, size));
        result.value.__set_a_f32vector(std::vector<double>(start, start + size));
        return result;
    } else if (pmt::is_f64vector(knob)) {
        GNURadio::Knob result;
        result.type = GNURadio::BaseTypes::F64VECTOR;
        size_t size(pmt::length(knob));
        const double* start((const double*)pmt::f64vector_elements(knob, size));
        result.value.__set_a_f64vector(std::vector<double>(start, start + size));
        return result;
    } else if (pmt::is_s64vector(knob)) {
        GNURadio::Knob result;
        result.type = GNURadio::BaseTypes::S64VECTOR;
        size_t size(pmt::length(knob));
        const int64_t* start((const int64_t*)pmt::s64vector_elements(knob, size));
        result.value.__set_a_s64vector(std::vector<int64_t>(start, start + size));
        return result;
    } else if (pmt::is_s32vector(knob)) {
        GNURadio::Knob result;
        result.type = GNURadio::BaseTypes::S32VECTOR;
        size_t size(pmt::length(knob));
        const int32_t* start((const int32_t*)pmt::s32vector_elements(knob, size));
        result.value.__set_a_s32vector(std::vector<int32_t>(start, start + size));
        return result;
    } else if (pmt::is_s16vector(knob)) {
        GNURadio::Knob result;
        result.type = GNURadio::BaseTypes::S16VECTOR;
        size_t size(pmt::length(knob));
        const int16_t* start((const int16_t*)pmt::s16vector_elements(knob, size));
        result.value.__set_a_s16vector(std::vector<int16_t>(start, start + size));
        return result;
    } else if (pmt::is_s8vector(knob)) {
        GNURadio::Knob result;
        result.type = GNURadio::BaseTypes::S8VECTOR;
        size_t size(pmt::length(knob));
        const int8_t* start((const int8_t*)pmt::s8vector_elements(knob, size));
        result.value.__set_a_s8vector(std::basic_string<char>(start, start + size));
        return result;
    } else if (pmt::is_c32vector(knob)) {
        std::vector<GNURadio::complex> z;

        GNURadio::Knob result;
        result.type = GNURadio::BaseTypes::C32VECTOR;
        size_t size(pmt::length(knob));
        const gr_complex* start((const gr_complex*)pmt::c32vector_elements(knob, size));
        for (size_t s = 0; s < size; s++) {
            GNURadio::complex z0;
            gr_complex z1 = gr_complex(*(start + s));
            z0.__set_re(z1.real());
            z0.__set_im(z1.imag());
            z.push_back(z0);
        }
        result.value.__set_a_c32vector(z);
        return result;
    } else {
        // FIXME: Don't get loggers every time we need to log something.
        gr::logger_ptr logger, debug_logger;
        gr::configure_default_loggers(logger, debug_logger, "rpcpmtconverter");
        logger->error("ERROR Don't know how to handle Knob Type (from): {}",
                      pmt::write_string(knob));
        assert(0);
    }
    return GNURadio::Knob();
}

pmt::pmt_t rpcpmtconverter::to_pmt_byte_f::operator()(const GNURadio::Knob& knob)
{
    return pmt::mp(knob.value.a_byte);
}

pmt::pmt_t rpcpmtconverter::to_pmt_short_f::operator()(const GNURadio::Knob& knob)
{
    return pmt::mp(knob.value.a_short);
}

pmt::pmt_t rpcpmtconverter::to_pmt_int_f::operator()(const GNURadio::Knob& knob)
{
    return pmt::mp(knob.value.a_int);
}

pmt::pmt_t rpcpmtconverter::to_pmt_long_f::operator()(const GNURadio::Knob& knob)
{
    return pmt::from_long(knob.value.a_long);
}

pmt::pmt_t rpcpmtconverter::to_pmt_double_f::operator()(const GNURadio::Knob& knob)
{
    return pmt::mp(knob.value.a_double);
}

pmt::pmt_t rpcpmtconverter::to_pmt_string_f::operator()(const GNURadio::Knob& knob)
{
    return pmt::string_to_symbol(knob.value.a_string);
}

pmt::pmt_t rpcpmtconverter::to_pmt_bool_f::operator()(const GNURadio::Knob& knob)
{
    if (knob.value.a_bool)
        return pmt::PMT_T;
    else
        return pmt::PMT_F;
}

pmt::pmt_t rpcpmtconverter::to_pmt_complex_f::operator()(const GNURadio::Knob& knob)
{
    gr_complexd cpx(knob.value.a_complex.re, knob.value.a_complex.im);
    return pmt::from_complex(cpx);
}

pmt::pmt_t rpcpmtconverter::to_pmt_f32vect_f::operator()(const GNURadio::Knob& knob)
{
    std::vector<double> v_double = knob.value.a_f32vector;
    std::vector<float> v(v_double.begin(), v_double.end());
    return pmt::init_f32vector(v.size(), v);
}

pmt::pmt_t rpcpmtconverter::to_pmt_f64vect_f::operator()(const GNURadio::Knob& knob)
{
    std::vector<double> v = knob.value.a_f64vector;
    return pmt::init_f64vector(v.size(), v);
}

pmt::pmt_t rpcpmtconverter::to_pmt_s64vect_f::operator()(const GNURadio::Knob& knob)
{
    std::vector<int64_t> v = knob.value.a_s64vector;
    return pmt::init_s64vector(v.size(), v);
}

pmt::pmt_t rpcpmtconverter::to_pmt_s32vect_f::operator()(const GNURadio::Knob& knob)
{
    std::vector<int32_t> v = knob.value.a_s32vector;
    return pmt::init_s32vector(v.size(), v);
}

pmt::pmt_t rpcpmtconverter::to_pmt_s16vect_f::operator()(const GNURadio::Knob& knob)
{
    std::vector<int16_t> v = knob.value.a_s16vector;
    return pmt::init_s16vector(v.size(), v);
}

pmt::pmt_t rpcpmtconverter::to_pmt_s8vect_f::operator()(const GNURadio::Knob& knob)
{
    std::basic_string<char> v = knob.value.a_s8vector;
    return pmt::init_s8vector(v.size(), reinterpret_cast<const int8_t*>(v.data()));
}

pmt::pmt_t rpcpmtconverter::to_pmt_c32vect_f::operator()(const GNURadio::Knob& knob)
{
    std::vector<GNURadio::complex> v0 = knob.value.a_c32vector;
    std::vector<GNURadio::complex>::iterator vitr;
    std::vector<gr_complex> v;
    for (vitr = v0.begin(); vitr != v0.end(); vitr++) {
        v.push_back(gr_complex(vitr->re, vitr->im));
    }
    return pmt::init_c32vector(v.size(), v);
}

rpcpmtconverter::To_PMT rpcpmtconverter::To_PMT::instance;

rpcpmtconverter::to_pmt_reg<rpcpmtconverter::to_pmt_bool_f>
    reg_bool(rpcpmtconverter::To_PMT::instance, GNURadio::BaseTypes::BOOL);
rpcpmtconverter::to_pmt_reg<rpcpmtconverter::to_pmt_byte_f>
    reg_byte(rpcpmtconverter::To_PMT::instance, GNURadio::BaseTypes::BYTE);
rpcpmtconverter::to_pmt_reg<rpcpmtconverter::to_pmt_short_f>
    reg_short(rpcpmtconverter::To_PMT::instance, GNURadio::BaseTypes::SHORT);
rpcpmtconverter::to_pmt_reg<rpcpmtconverter::to_pmt_int_f>
    reg_int(rpcpmtconverter::To_PMT::instance, GNURadio::BaseTypes::INT);
rpcpmtconverter::to_pmt_reg<rpcpmtconverter::to_pmt_long_f>
    reg_long(rpcpmtconverter::To_PMT::instance, GNURadio::BaseTypes::LONG);
rpcpmtconverter::to_pmt_reg<rpcpmtconverter::to_pmt_double_f>
    reg_double(rpcpmtconverter::To_PMT::instance, GNURadio::BaseTypes::DOUBLE);
rpcpmtconverter::to_pmt_reg<rpcpmtconverter::to_pmt_string_f>
    reg_string(rpcpmtconverter::To_PMT::instance, GNURadio::BaseTypes::STRING);
rpcpmtconverter::to_pmt_reg<rpcpmtconverter::to_pmt_complex_f>
    reg_complex(rpcpmtconverter::To_PMT::instance, GNURadio::BaseTypes::COMPLEX);
rpcpmtconverter::to_pmt_reg<rpcpmtconverter::to_pmt_f32vect_f>
    reg_f32v(rpcpmtconverter::To_PMT::instance, GNURadio::BaseTypes::F32VECTOR);
rpcpmtconverter::to_pmt_reg<rpcpmtconverter::to_pmt_f64vect_f>
    reg_f64v(rpcpmtconverter::To_PMT::instance, GNURadio::BaseTypes::F64VECTOR);
rpcpmtconverter::to_pmt_reg<rpcpmtconverter::to_pmt_s64vect_f>
    reg_s64v(rpcpmtconverter::To_PMT::instance, GNURadio::BaseTypes::S64VECTOR);
rpcpmtconverter::to_pmt_reg<rpcpmtconverter::to_pmt_s32vect_f>
    reg_s32v(rpcpmtconverter::To_PMT::instance, GNURadio::BaseTypes::S32VECTOR);
rpcpmtconverter::to_pmt_reg<rpcpmtconverter::to_pmt_s16vect_f>
    reg_s16v(rpcpmtconverter::To_PMT::instance, GNURadio::BaseTypes::S16VECTOR);
rpcpmtconverter::to_pmt_reg<rpcpmtconverter::to_pmt_s8vect_f>
    reg_s8v(rpcpmtconverter::To_PMT::instance, GNURadio::BaseTypes::S8VECTOR);
rpcpmtconverter::to_pmt_reg<rpcpmtconverter::to_pmt_c32vect_f>
    reg_c32v(rpcpmtconverter::To_PMT::instance, GNURadio::BaseTypes::C32VECTOR);

template <typename TO_PMT_F>
rpcpmtconverter::to_pmt_reg<TO_PMT_F>::to_pmt_reg(To_PMT& instance,
                                                  const GNURadio::BaseTypes::type type)
{
    instance.to_pmt_map.emplace(type, TO_PMT_F());
}

pmt::pmt_t rpcpmtconverter::to_pmt_f::operator()(const GNURadio::Knob& knob)
{
    // FIXME: Don't get loggers every time we need to log something.
    gr::logger_ptr logger, debug_logger;
    gr::configure_default_loggers(logger, debug_logger, "rpcpmtconverter");
    debug_logger->error("ERROR Don't know how to handle Knob Type (from): {}",
                        to_string(knob.type));
    assert(0);
    return pmt::pmt_t();
}

pmt::pmt_t rpcpmtconverter::To_PMT::operator()(const GNURadio::Knob& knob)
{
    return to_pmt_map[knob.type](knob);
}
