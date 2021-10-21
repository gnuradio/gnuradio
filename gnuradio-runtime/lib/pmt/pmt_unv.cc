/* -*- c++ -*- */
/*
 * Copyright 2006,2009,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

// This file is machine generated using the generate_unv.py tool

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "pmt_int.h"
#include "pmt_unv_int.h"
#include <pmt/pmt.h>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
////////////////////////////////////////////////////////////////////////////
//                           pmt_u8vector
////////////////////////////////////////////////////////////////////////////

namespace pmt {

static pmt_u8vector* _u8vector(pmt_t x) { return dynamic_cast<pmt_u8vector*>(x.get()); }


pmt_u8vector::pmt_u8vector(size_t k, uint8_t fill) : d_v(k)
{
    for (size_t i = 0; i < k; i++)
        d_v[i] = fill;
}

pmt_u8vector::pmt_u8vector(size_t k, const uint8_t* data) : d_v(k)
{
    if (k) {
        std::memcpy(d_v.data(), data, k * sizeof(uint8_t));
    }
}


uint8_t pmt_u8vector::ref(size_t k) const
{
    if (k >= length())
        throw out_of_range("pmt_u8vector_ref", from_long(k));
    return d_v[k];
}

void pmt_u8vector::set(size_t k, uint8_t x)
{
    if (k >= length())
        throw out_of_range("pmt_u8vector_set", from_long(k));
    d_v[k] = x;
}

const uint8_t* pmt_u8vector::elements(size_t& len)
{
    len = length();
    return len ? d_v.data() : nullptr;
}

uint8_t* pmt_u8vector::writable_elements(size_t& len)
{
    len = length();
    return len ? d_v.data() : nullptr;
}

const void* pmt_u8vector::uniform_elements(size_t& len)
{
    len = length() * sizeof(uint8_t);
    return len ? d_v.data() : nullptr;
}

void* pmt_u8vector::uniform_writable_elements(size_t& len)
{
    len = length() * sizeof(uint8_t);
    return len ? (d_v.data()) : nullptr;
}

bool is_u8vector(pmt_t obj) { return obj->is_u8vector(); }

pmt_t make_u8vector(size_t k, uint8_t fill) { return pmt_t(new pmt_u8vector(k, fill)); }

pmt_t init_u8vector(size_t k, const uint8_t* data)
{
    return pmt_t(new pmt_u8vector(k, data));
}

pmt_t init_u8vector(size_t k, const std::vector<uint8_t>& data)
{
    if (k) {
        return pmt_t(new pmt_u8vector(k, &data[0]));
    }
    return pmt_t(
        new pmt_u8vector(k, static_cast<uint8_t>(0))); // fills an empty vector with 0
}

uint8_t u8vector_ref(pmt_t vector, size_t k)
{
    if (!vector->is_u8vector())
        throw wrong_type("pmt_u8vector_ref", vector);
    return _u8vector(vector)->ref(k);
}

void u8vector_set(pmt_t vector, size_t k, uint8_t obj)
{
    if (!vector->is_u8vector())
        throw wrong_type("pmt_u8vector_set", vector);
    _u8vector(vector)->set(k, obj);
}

const uint8_t* u8vector_elements(pmt_t vector, size_t& len)
{
    if (!vector->is_u8vector())
        throw wrong_type("pmt_u8vector_elements", vector);
    return _u8vector(vector)->elements(len);
}

const std::vector<uint8_t> u8vector_elements(pmt_t vector)
{
    if (!vector->is_u8vector())
        throw wrong_type("pmt_u8vector_elements", vector);
    size_t len;
    const uint8_t* array = _u8vector(vector)->elements(len);
    const std::vector<uint8_t> vec(array, array + len);
    return vec;
}


uint8_t* u8vector_writable_elements(pmt_t vector, size_t& len)
{
    if (!vector->is_u8vector())
        throw wrong_type("pmt_u8vector_writable_elements", vector);
    return _u8vector(vector)->writable_elements(len);
}

const std::string pmt_u8vector::string_ref(size_t k) const
{
    const auto& ch = ref(k);
    return std::to_string(ch);
}

} /* namespace pmt */
////////////////////////////////////////////////////////////////////////////
//                           pmt_s8vector
////////////////////////////////////////////////////////////////////////////

namespace pmt {

static pmt_s8vector* _s8vector(pmt_t x) { return dynamic_cast<pmt_s8vector*>(x.get()); }


pmt_s8vector::pmt_s8vector(size_t k, int8_t fill) : d_v(k)
{
    for (size_t i = 0; i < k; i++)
        d_v[i] = fill;
}

pmt_s8vector::pmt_s8vector(size_t k, const int8_t* data) : d_v(k)
{
    if (k)
        std::memcpy(d_v.data(), data, k * sizeof(int8_t));
}

int8_t pmt_s8vector::ref(size_t k) const
{
    if (k >= length())
        throw out_of_range("pmt_s8vector_ref", from_long(k));
    return d_v[k];
}

void pmt_s8vector::set(size_t k, int8_t x)
{
    if (k >= length())
        throw out_of_range("pmt_s8vector_set", from_long(k));
    d_v[k] = x;
}

const int8_t* pmt_s8vector::elements(size_t& len)
{
    len = length();
    return len ? d_v.data() : nullptr;
}

int8_t* pmt_s8vector::writable_elements(size_t& len)
{
    len = length();
    return len ? d_v.data() : nullptr;
}

const void* pmt_s8vector::uniform_elements(size_t& len)
{
    len = length() * sizeof(int8_t);
    return len ? d_v.data() : nullptr;
}

void* pmt_s8vector::uniform_writable_elements(size_t& len)
{
    len = length() * sizeof(int8_t);
    return len ? (d_v.data()) : nullptr;
}

bool is_s8vector(pmt_t obj) { return obj->is_s8vector(); }

pmt_t make_s8vector(size_t k, int8_t fill) { return pmt_t(new pmt_s8vector(k, fill)); }

pmt_t init_s8vector(size_t k, const int8_t* data)
{
    return pmt_t(new pmt_s8vector(k, data));
}

pmt_t init_s8vector(size_t k, const std::vector<int8_t>& data)
{
    if (k) {
        return pmt_t(new pmt_s8vector(k, &data[0]));
    }
    return pmt_t(
        new pmt_s8vector(k, static_cast<int8_t>(0))); // fills an empty vector with 0
}

int8_t s8vector_ref(pmt_t vector, size_t k)
{
    if (!vector->is_s8vector())
        throw wrong_type("pmt_s8vector_ref", vector);
    return _s8vector(vector)->ref(k);
}

void s8vector_set(pmt_t vector, size_t k, int8_t obj)
{
    if (!vector->is_s8vector())
        throw wrong_type("pmt_s8vector_set", vector);
    _s8vector(vector)->set(k, obj);
}

const int8_t* s8vector_elements(pmt_t vector, size_t& len)
{
    if (!vector->is_s8vector())
        throw wrong_type("pmt_s8vector_elements", vector);
    return _s8vector(vector)->elements(len);
}

const std::vector<int8_t> s8vector_elements(pmt_t vector)
{
    if (!vector->is_s8vector())
        throw wrong_type("pmt_s8vector_elements", vector);
    size_t len;
    const int8_t* array = _s8vector(vector)->elements(len);
    const std::vector<int8_t> vec(array, array + len);
    return vec;
}


int8_t* s8vector_writable_elements(pmt_t vector, size_t& len)
{
    if (!vector->is_s8vector())
        throw wrong_type("pmt_s8vector_writable_elements", vector);
    return _s8vector(vector)->writable_elements(len);
}

const std::string pmt_s8vector::string_ref(size_t k) const
{
    const auto& ch = ref(k);
    return std::to_string(ch);
}

} /* namespace pmt */
////////////////////////////////////////////////////////////////////////////
//                           pmt_u16vector
////////////////////////////////////////////////////////////////////////////

namespace pmt {

static pmt_u16vector* _u16vector(pmt_t x)
{
    return dynamic_cast<pmt_u16vector*>(x.get());
}


pmt_u16vector::pmt_u16vector(size_t k, uint16_t fill) : d_v(k)
{
    for (size_t i = 0; i < k; i++)
        d_v[i] = fill;
}

pmt_u16vector::pmt_u16vector(size_t k, const uint16_t* data) : d_v(k)
{
    if (k)
        std::memcpy(d_v.data(), data, k * sizeof(uint16_t));
}

uint16_t pmt_u16vector::ref(size_t k) const
{
    if (k >= length())
        throw out_of_range("pmt_u16vector_ref", from_long(k));
    return d_v[k];
}

void pmt_u16vector::set(size_t k, uint16_t x)
{
    if (k >= length())
        throw out_of_range("pmt_u16vector_set", from_long(k));
    d_v[k] = x;
}

const uint16_t* pmt_u16vector::elements(size_t& len)
{
    len = length();
    return len ? d_v.data() : nullptr;
}

uint16_t* pmt_u16vector::writable_elements(size_t& len)
{
    len = length();
    return len ? d_v.data() : nullptr;
}

const void* pmt_u16vector::uniform_elements(size_t& len)
{
    len = length() * sizeof(uint16_t);
    return len ? d_v.data() : nullptr;
}

void* pmt_u16vector::uniform_writable_elements(size_t& len)
{
    len = length() * sizeof(uint16_t);
    return len ? (d_v.data()) : nullptr;
}

bool is_u16vector(pmt_t obj) { return obj->is_u16vector(); }

pmt_t make_u16vector(size_t k, uint16_t fill)
{
    return pmt_t(new pmt_u16vector(k, fill));
}

pmt_t init_u16vector(size_t k, const uint16_t* data)
{
    return pmt_t(new pmt_u16vector(k, data));
}

pmt_t init_u16vector(size_t k, const std::vector<uint16_t>& data)
{
    if (k) {
        return pmt_t(new pmt_u16vector(k, &data[0]));
    }
    return pmt_t(
        new pmt_u16vector(k, static_cast<uint16_t>(0))); // fills an empty vector with 0
}

uint16_t u16vector_ref(pmt_t vector, size_t k)
{
    if (!vector->is_u16vector())
        throw wrong_type("pmt_u16vector_ref", vector);
    return _u16vector(vector)->ref(k);
}

void u16vector_set(pmt_t vector, size_t k, uint16_t obj)
{
    if (!vector->is_u16vector())
        throw wrong_type("pmt_u16vector_set", vector);
    _u16vector(vector)->set(k, obj);
}

const uint16_t* u16vector_elements(pmt_t vector, size_t& len)
{
    if (!vector->is_u16vector())
        throw wrong_type("pmt_u16vector_elements", vector);
    return _u16vector(vector)->elements(len);
}

const std::vector<uint16_t> u16vector_elements(pmt_t vector)
{
    if (!vector->is_u16vector())
        throw wrong_type("pmt_u16vector_elements", vector);
    size_t len;
    const uint16_t* array = _u16vector(vector)->elements(len);
    const std::vector<uint16_t> vec(array, array + len);
    return vec;
}


uint16_t* u16vector_writable_elements(pmt_t vector, size_t& len)
{
    if (!vector->is_u16vector())
        throw wrong_type("pmt_u16vector_writable_elements", vector);
    return _u16vector(vector)->writable_elements(len);
}

const std::string pmt_u16vector::string_ref(size_t k) const
{
    return std::to_string(ref(k));
}

} /* namespace pmt */
////////////////////////////////////////////////////////////////////////////
//                           pmt_s16vector
////////////////////////////////////////////////////////////////////////////

namespace pmt {

static pmt_s16vector* _s16vector(pmt_t x)
{
    return dynamic_cast<pmt_s16vector*>(x.get());
}


pmt_s16vector::pmt_s16vector(size_t k, int16_t fill) : d_v(k)
{
    for (size_t i = 0; i < k; i++)
        d_v[i] = fill;
}

pmt_s16vector::pmt_s16vector(size_t k, const int16_t* data) : d_v(k)
{
    if (k)
        std::memcpy(d_v.data(), data, k * sizeof(int16_t));
}

int16_t pmt_s16vector::ref(size_t k) const
{
    if (k >= length())
        throw out_of_range("pmt_s16vector_ref", from_long(k));
    return d_v[k];
}

void pmt_s16vector::set(size_t k, int16_t x)
{
    if (k >= length())
        throw out_of_range("pmt_s16vector_set", from_long(k));
    d_v[k] = x;
}

const int16_t* pmt_s16vector::elements(size_t& len)
{
    len = length();
    return len ? d_v.data() : nullptr;
}

int16_t* pmt_s16vector::writable_elements(size_t& len)
{
    len = length();
    return len ? d_v.data() : nullptr;
}

const void* pmt_s16vector::uniform_elements(size_t& len)
{
    len = length() * sizeof(int16_t);
    return len ? d_v.data() : nullptr;
}

void* pmt_s16vector::uniform_writable_elements(size_t& len)
{
    len = length() * sizeof(int16_t);
    return len ? (d_v.data()) : nullptr;
}

bool is_s16vector(pmt_t obj) { return obj->is_s16vector(); }

pmt_t make_s16vector(size_t k, int16_t fill) { return pmt_t(new pmt_s16vector(k, fill)); }

pmt_t init_s16vector(size_t k, const int16_t* data)
{
    return pmt_t(new pmt_s16vector(k, data));
}

pmt_t init_s16vector(size_t k, const std::vector<int16_t>& data)
{
    if (k) {
        return pmt_t(new pmt_s16vector(k, &data[0]));
    }
    return pmt_t(
        new pmt_s16vector(k, static_cast<int16_t>(0))); // fills an empty vector with 0
}

int16_t s16vector_ref(pmt_t vector, size_t k)
{
    if (!vector->is_s16vector())
        throw wrong_type("pmt_s16vector_ref", vector);
    return _s16vector(vector)->ref(k);
}

void s16vector_set(pmt_t vector, size_t k, int16_t obj)
{
    if (!vector->is_s16vector())
        throw wrong_type("pmt_s16vector_set", vector);
    _s16vector(vector)->set(k, obj);
}

const int16_t* s16vector_elements(pmt_t vector, size_t& len)
{
    if (!vector->is_s16vector())
        throw wrong_type("pmt_s16vector_elements", vector);
    return _s16vector(vector)->elements(len);
}

const std::vector<int16_t> s16vector_elements(pmt_t vector)
{
    if (!vector->is_s16vector())
        throw wrong_type("pmt_s16vector_elements", vector);
    size_t len;
    const int16_t* array = _s16vector(vector)->elements(len);
    const std::vector<int16_t> vec(array, array + len);
    return vec;
}


int16_t* s16vector_writable_elements(pmt_t vector, size_t& len)
{
    if (!vector->is_s16vector())
        throw wrong_type("pmt_s16vector_writable_elements", vector);
    return _s16vector(vector)->writable_elements(len);
}

const std::string pmt_s16vector::string_ref(size_t k) const
{
    return std::to_string(ref(k));
}

} /* namespace pmt */
////////////////////////////////////////////////////////////////////////////
//                           pmt_u32vector
////////////////////////////////////////////////////////////////////////////

namespace pmt {

static pmt_u32vector* _u32vector(pmt_t x)
{
    return dynamic_cast<pmt_u32vector*>(x.get());
}


pmt_u32vector::pmt_u32vector(size_t k, uint32_t fill) : d_v(k)
{
    for (size_t i = 0; i < k; i++)
        d_v[i] = fill;
}

pmt_u32vector::pmt_u32vector(size_t k, const uint32_t* data) : d_v(k)
{
    if (k)
        std::memcpy(d_v.data(), data, k * sizeof(uint32_t));
}

uint32_t pmt_u32vector::ref(size_t k) const
{
    if (k >= length())
        throw out_of_range("pmt_u32vector_ref", from_long(k));
    return d_v[k];
}

void pmt_u32vector::set(size_t k, uint32_t x)
{
    if (k >= length())
        throw out_of_range("pmt_u32vector_set", from_long(k));
    d_v[k] = x;
}

const uint32_t* pmt_u32vector::elements(size_t& len)
{
    len = length();
    return len ? d_v.data() : nullptr;
}

uint32_t* pmt_u32vector::writable_elements(size_t& len)
{
    len = length();
    return len ? d_v.data() : nullptr;
}

const void* pmt_u32vector::uniform_elements(size_t& len)
{
    len = length() * sizeof(uint32_t);
    return len ? d_v.data() : nullptr;
}

void* pmt_u32vector::uniform_writable_elements(size_t& len)
{
    len = length() * sizeof(uint32_t);
    return len ? (d_v.data()) : nullptr;
}

bool is_u32vector(pmt_t obj) { return obj->is_u32vector(); }

pmt_t make_u32vector(size_t k, uint32_t fill)
{
    return pmt_t(new pmt_u32vector(k, fill));
}

pmt_t init_u32vector(size_t k, const uint32_t* data)
{
    return pmt_t(new pmt_u32vector(k, data));
}

pmt_t init_u32vector(size_t k, const std::vector<uint32_t>& data)
{
    if (k) {
        return pmt_t(new pmt_u32vector(k, &data[0]));
    }
    return pmt_t(
        new pmt_u32vector(k, static_cast<uint32_t>(0))); // fills an empty vector with 0
}

uint32_t u32vector_ref(pmt_t vector, size_t k)
{
    if (!vector->is_u32vector())
        throw wrong_type("pmt_u32vector_ref", vector);
    return _u32vector(vector)->ref(k);
}

void u32vector_set(pmt_t vector, size_t k, uint32_t obj)
{
    if (!vector->is_u32vector())
        throw wrong_type("pmt_u32vector_set", vector);
    _u32vector(vector)->set(k, obj);
}

const uint32_t* u32vector_elements(pmt_t vector, size_t& len)
{
    if (!vector->is_u32vector())
        throw wrong_type("pmt_u32vector_elements", vector);
    return _u32vector(vector)->elements(len);
}

const std::vector<uint32_t> u32vector_elements(pmt_t vector)
{
    if (!vector->is_u32vector())
        throw wrong_type("pmt_u32vector_elements", vector);
    size_t len;
    const uint32_t* array = _u32vector(vector)->elements(len);
    const std::vector<uint32_t> vec(array, array + len);
    return vec;
}


uint32_t* u32vector_writable_elements(pmt_t vector, size_t& len)
{
    if (!vector->is_u32vector())
        throw wrong_type("pmt_u32vector_writable_elements", vector);
    return _u32vector(vector)->writable_elements(len);
}

const std::string pmt_u32vector::string_ref(size_t k) const
{
    return std::to_string(ref(k));
}

} /* namespace pmt */
////////////////////////////////////////////////////////////////////////////
//                           pmt_s32vector
////////////////////////////////////////////////////////////////////////////

namespace pmt {

static pmt_s32vector* _s32vector(pmt_t x)
{
    return dynamic_cast<pmt_s32vector*>(x.get());
}


pmt_s32vector::pmt_s32vector(size_t k, int32_t fill) : d_v(k)
{
    for (size_t i = 0; i < k; i++)
        d_v[i] = fill;
}

pmt_s32vector::pmt_s32vector(size_t k, const int32_t* data) : d_v(k)
{
    if (k)
        std::memcpy(d_v.data(), data, k * sizeof(int32_t));
}

int32_t pmt_s32vector::ref(size_t k) const
{
    if (k >= length())
        throw out_of_range("pmt_s32vector_ref", from_long(k));
    return d_v[k];
}

void pmt_s32vector::set(size_t k, int32_t x)
{
    if (k >= length())
        throw out_of_range("pmt_s32vector_set", from_long(k));
    d_v[k] = x;
}

const int32_t* pmt_s32vector::elements(size_t& len)
{
    len = length();
    return len ? d_v.data() : nullptr;
}

int32_t* pmt_s32vector::writable_elements(size_t& len)
{
    len = length();
    return len ? d_v.data() : nullptr;
}

const void* pmt_s32vector::uniform_elements(size_t& len)
{
    len = length() * sizeof(int32_t);
    return len ? d_v.data() : nullptr;
}

void* pmt_s32vector::uniform_writable_elements(size_t& len)
{
    len = length() * sizeof(int32_t);
    return len ? (d_v.data()) : nullptr;
}

bool is_s32vector(pmt_t obj) { return obj->is_s32vector(); }

pmt_t make_s32vector(size_t k, int32_t fill) { return pmt_t(new pmt_s32vector(k, fill)); }

pmt_t init_s32vector(size_t k, const int32_t* data)
{
    return pmt_t(new pmt_s32vector(k, data));
}

pmt_t init_s32vector(size_t k, const std::vector<int32_t>& data)
{
    if (k) {
        return pmt_t(new pmt_s32vector(k, &data[0]));
    }
    return pmt_t(
        new pmt_s32vector(k, static_cast<int32_t>(0))); // fills an empty vector with 0
}

int32_t s32vector_ref(pmt_t vector, size_t k)
{
    if (!vector->is_s32vector())
        throw wrong_type("pmt_s32vector_ref", vector);
    return _s32vector(vector)->ref(k);
}

void s32vector_set(pmt_t vector, size_t k, int32_t obj)
{
    if (!vector->is_s32vector())
        throw wrong_type("pmt_s32vector_set", vector);
    _s32vector(vector)->set(k, obj);
}

const int32_t* s32vector_elements(pmt_t vector, size_t& len)
{
    if (!vector->is_s32vector())
        throw wrong_type("pmt_s32vector_elements", vector);
    return _s32vector(vector)->elements(len);
}

const std::vector<int32_t> s32vector_elements(pmt_t vector)
{
    if (!vector->is_s32vector())
        throw wrong_type("pmt_s32vector_elements", vector);
    size_t len;
    const int32_t* array = _s32vector(vector)->elements(len);
    const std::vector<int32_t> vec(array, array + len);
    return vec;
}


int32_t* s32vector_writable_elements(pmt_t vector, size_t& len)
{
    if (!vector->is_s32vector())
        throw wrong_type("pmt_s32vector_writable_elements", vector);
    return _s32vector(vector)->writable_elements(len);
}

const std::string pmt_s32vector::string_ref(size_t k) const
{
    return std::to_string(ref(k));
}

} /* namespace pmt */
////////////////////////////////////////////////////////////////////////////
//                           pmt_u64vector
////////////////////////////////////////////////////////////////////////////

namespace pmt {

static pmt_u64vector* _u64vector(pmt_t x)
{
    return dynamic_cast<pmt_u64vector*>(x.get());
}


pmt_u64vector::pmt_u64vector(size_t k, uint64_t fill) : d_v(k)
{
    for (size_t i = 0; i < k; i++)
        d_v[i] = fill;
}

pmt_u64vector::pmt_u64vector(size_t k, const uint64_t* data) : d_v(k)
{
    if (k)
        std::memcpy(d_v.data(), data, k * sizeof(uint64_t));
}

uint64_t pmt_u64vector::ref(size_t k) const
{
    if (k >= length())
        throw out_of_range("pmt_u64vector_ref", from_long(k));
    return d_v[k];
}

void pmt_u64vector::set(size_t k, uint64_t x)
{
    if (k >= length())
        throw out_of_range("pmt_u64vector_set", from_long(k));
    d_v[k] = x;
}

const uint64_t* pmt_u64vector::elements(size_t& len)
{
    len = length();
    return len ? d_v.data() : nullptr;
}

uint64_t* pmt_u64vector::writable_elements(size_t& len)
{
    len = length();
    return len ? d_v.data() : nullptr;
}

const void* pmt_u64vector::uniform_elements(size_t& len)
{
    len = length() * sizeof(uint64_t);
    return len ? d_v.data() : nullptr;
}

void* pmt_u64vector::uniform_writable_elements(size_t& len)
{
    len = length() * sizeof(uint64_t);
    return len ? (d_v.data()) : nullptr;
}

bool is_u64vector(pmt_t obj) { return obj->is_u64vector(); }

pmt_t make_u64vector(size_t k, uint64_t fill)
{
    return pmt_t(new pmt_u64vector(k, fill));
}

pmt_t init_u64vector(size_t k, const uint64_t* data)
{
    return pmt_t(new pmt_u64vector(k, data));
}

pmt_t init_u64vector(size_t k, const std::vector<uint64_t>& data)
{
    if (k) {
        return pmt_t(new pmt_u64vector(k, &data[0]));
    }
    return pmt_t(
        new pmt_u64vector(k, static_cast<uint64_t>(0))); // fills an empty vector with 0
}

uint64_t u64vector_ref(pmt_t vector, size_t k)
{
    if (!vector->is_u64vector())
        throw wrong_type("pmt_u64vector_ref", vector);
    return _u64vector(vector)->ref(k);
}

void u64vector_set(pmt_t vector, size_t k, uint64_t obj)
{
    if (!vector->is_u64vector())
        throw wrong_type("pmt_u64vector_set", vector);
    _u64vector(vector)->set(k, obj);
}

const uint64_t* u64vector_elements(pmt_t vector, size_t& len)
{
    if (!vector->is_u64vector())
        throw wrong_type("pmt_u64vector_elements", vector);
    return _u64vector(vector)->elements(len);
}

const std::vector<uint64_t> u64vector_elements(pmt_t vector)
{
    if (!vector->is_u64vector())
        throw wrong_type("pmt_u64vector_elements", vector);
    size_t len;
    const uint64_t* array = _u64vector(vector)->elements(len);
    const std::vector<uint64_t> vec(array, array + len);
    return vec;
}


uint64_t* u64vector_writable_elements(pmt_t vector, size_t& len)
{
    if (!vector->is_u64vector())
        throw wrong_type("pmt_u64vector_writable_elements", vector);
    return _u64vector(vector)->writable_elements(len);
}

const std::string pmt_u64vector::string_ref(size_t k) const
{
    return std::to_string(ref(k));
}

} /* namespace pmt */
////////////////////////////////////////////////////////////////////////////
//                           pmt_s64vector
////////////////////////////////////////////////////////////////////////////

namespace pmt {

static pmt_s64vector* _s64vector(pmt_t x)
{
    return dynamic_cast<pmt_s64vector*>(x.get());
}


pmt_s64vector::pmt_s64vector(size_t k, int64_t fill) : d_v(k)
{
    for (size_t i = 0; i < k; i++)
        d_v[i] = fill;
}

pmt_s64vector::pmt_s64vector(size_t k, const int64_t* data) : d_v(k)
{
    if (k)
        std::memcpy(d_v.data(), data, k * sizeof(int64_t));
}

int64_t pmt_s64vector::ref(size_t k) const
{
    if (k >= length())
        throw out_of_range("pmt_s64vector_ref", from_long(k));
    return d_v[k];
}

void pmt_s64vector::set(size_t k, int64_t x)
{
    if (k >= length())
        throw out_of_range("pmt_s64vector_set", from_long(k));
    d_v[k] = x;
}

const int64_t* pmt_s64vector::elements(size_t& len)
{
    len = length();
    return len ? d_v.data() : nullptr;
}

int64_t* pmt_s64vector::writable_elements(size_t& len)
{
    len = length();
    return len ? d_v.data() : nullptr;
}

const void* pmt_s64vector::uniform_elements(size_t& len)
{
    len = length() * sizeof(int64_t);
    return len ? d_v.data() : nullptr;
}

void* pmt_s64vector::uniform_writable_elements(size_t& len)
{
    len = length() * sizeof(int64_t);
    return len ? (d_v.data()) : nullptr;
}

bool is_s64vector(pmt_t obj) { return obj->is_s64vector(); }

pmt_t make_s64vector(size_t k, int64_t fill) { return pmt_t(new pmt_s64vector(k, fill)); }

pmt_t init_s64vector(size_t k, const int64_t* data)
{
    return pmt_t(new pmt_s64vector(k, data));
}

pmt_t init_s64vector(size_t k, const std::vector<int64_t>& data)
{
    if (k) {
        return pmt_t(new pmt_s64vector(k, &data[0]));
    }
    return pmt_t(
        new pmt_s64vector(k, static_cast<int64_t>(0))); // fills an empty vector with 0
}

int64_t s64vector_ref(pmt_t vector, size_t k)
{
    if (!vector->is_s64vector())
        throw wrong_type("pmt_s64vector_ref", vector);
    return _s64vector(vector)->ref(k);
}

void s64vector_set(pmt_t vector, size_t k, int64_t obj)
{
    if (!vector->is_s64vector())
        throw wrong_type("pmt_s64vector_set", vector);
    _s64vector(vector)->set(k, obj);
}

const int64_t* s64vector_elements(pmt_t vector, size_t& len)
{
    if (!vector->is_s64vector())
        throw wrong_type("pmt_s64vector_elements", vector);
    return _s64vector(vector)->elements(len);
}

const std::vector<int64_t> s64vector_elements(pmt_t vector)
{
    if (!vector->is_s64vector())
        throw wrong_type("pmt_s64vector_elements", vector);
    size_t len;
    const int64_t* array = _s64vector(vector)->elements(len);
    const std::vector<int64_t> vec(array, array + len);
    return vec;
}


int64_t* s64vector_writable_elements(pmt_t vector, size_t& len)
{
    if (!vector->is_s64vector())
        throw wrong_type("pmt_s64vector_writable_elements", vector);
    return _s64vector(vector)->writable_elements(len);
}

const std::string pmt_s64vector::string_ref(size_t k) const
{
    return std::to_string(ref(k));
}

} /* namespace pmt */
////////////////////////////////////////////////////////////////////////////
//                           pmt_f32vector
////////////////////////////////////////////////////////////////////////////

namespace pmt {

static pmt_f32vector* _f32vector(pmt_t x)
{
    return dynamic_cast<pmt_f32vector*>(x.get());
}


pmt_f32vector::pmt_f32vector(size_t k, float fill) : d_v(k)
{
    for (size_t i = 0; i < k; i++)
        d_v[i] = fill;
}

pmt_f32vector::pmt_f32vector(size_t k, const float* data) : d_v(k)
{
    if (k)
        std::memcpy(d_v.data(), data, k * sizeof(float));
}

float pmt_f32vector::ref(size_t k) const
{
    if (k >= length())
        throw out_of_range("pmt_f32vector_ref", from_long(k));
    return d_v[k];
}

void pmt_f32vector::set(size_t k, float x)
{
    if (k >= length())
        throw out_of_range("pmt_f32vector_set", from_long(k));
    d_v[k] = x;
}

const float* pmt_f32vector::elements(size_t& len)
{
    len = length();
    return len ? d_v.data() : nullptr;
}

float* pmt_f32vector::writable_elements(size_t& len)
{
    len = length();
    return len ? d_v.data() : nullptr;
}

const void* pmt_f32vector::uniform_elements(size_t& len)
{
    len = length() * sizeof(float);
    return len ? d_v.data() : nullptr;
}

void* pmt_f32vector::uniform_writable_elements(size_t& len)
{
    len = length() * sizeof(float);
    return len ? (d_v.data()) : nullptr;
}

bool is_f32vector(pmt_t obj) { return obj->is_f32vector(); }

pmt_t make_f32vector(size_t k, float fill) { return pmt_t(new pmt_f32vector(k, fill)); }

pmt_t init_f32vector(size_t k, const float* data)
{
    return pmt_t(new pmt_f32vector(k, data));
}

pmt_t init_f32vector(size_t k, const std::vector<float>& data)
{
    if (k) {
        return pmt_t(new pmt_f32vector(k, &data[0]));
    }
    return pmt_t(
        new pmt_f32vector(k, static_cast<float>(0))); // fills an empty vector with 0
}

float f32vector_ref(pmt_t vector, size_t k)
{
    if (!vector->is_f32vector())
        throw wrong_type("pmt_f32vector_ref", vector);
    return _f32vector(vector)->ref(k);
}

void f32vector_set(pmt_t vector, size_t k, float obj)
{
    if (!vector->is_f32vector())
        throw wrong_type("pmt_f32vector_set", vector);
    _f32vector(vector)->set(k, obj);
}

const float* f32vector_elements(pmt_t vector, size_t& len)
{
    if (!vector->is_f32vector())
        throw wrong_type("pmt_f32vector_elements", vector);
    return _f32vector(vector)->elements(len);
}

const std::vector<float> f32vector_elements(pmt_t vector)
{
    if (!vector->is_f32vector())
        throw wrong_type("pmt_f32vector_elements", vector);
    size_t len;
    const float* array = _f32vector(vector)->elements(len);
    const std::vector<float> vec(array, array + len);
    return vec;
}


float* f32vector_writable_elements(pmt_t vector, size_t& len)
{
    if (!vector->is_f32vector())
        throw wrong_type("pmt_f32vector_writable_elements", vector);
    return _f32vector(vector)->writable_elements(len);
}

const std::string pmt_f32vector::string_ref(size_t k) const
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(std::numeric_limits<float>::digits10) << ref(k);
    return ss.str();
}

} /* namespace pmt */
////////////////////////////////////////////////////////////////////////////
//                           pmt_f64vector
////////////////////////////////////////////////////////////////////////////

namespace pmt {

static pmt_f64vector* _f64vector(pmt_t x)
{
    return dynamic_cast<pmt_f64vector*>(x.get());
}


pmt_f64vector::pmt_f64vector(size_t k, double fill) : d_v(k)
{
    for (size_t i = 0; i < k; i++)
        d_v[i] = fill;
}

pmt_f64vector::pmt_f64vector(size_t k, const double* data) : d_v(k)
{
    if (k)
        std::memcpy(d_v.data(), data, k * sizeof(double));
}

double pmt_f64vector::ref(size_t k) const
{
    if (k >= length())
        throw out_of_range("pmt_f64vector_ref", from_long(k));
    return d_v[k];
}

void pmt_f64vector::set(size_t k, double x)
{
    if (k >= length())
        throw out_of_range("pmt_f64vector_set", from_long(k));
    d_v[k] = x;
}

const double* pmt_f64vector::elements(size_t& len)
{
    len = length();
    return len ? d_v.data() : nullptr;
}

double* pmt_f64vector::writable_elements(size_t& len)
{
    len = length();
    return len ? d_v.data() : nullptr;
}

const void* pmt_f64vector::uniform_elements(size_t& len)
{
    len = length() * sizeof(double);
    return len ? d_v.data() : nullptr;
}

void* pmt_f64vector::uniform_writable_elements(size_t& len)
{
    len = length() * sizeof(double);
    return len ? (d_v.data()) : nullptr;
}

bool is_f64vector(pmt_t obj) { return obj->is_f64vector(); }

pmt_t make_f64vector(size_t k, double fill) { return pmt_t(new pmt_f64vector(k, fill)); }

pmt_t init_f64vector(size_t k, const double* data)
{
    return pmt_t(new pmt_f64vector(k, data));
}

pmt_t init_f64vector(size_t k, const std::vector<double>& data)
{
    if (k) {
        return pmt_t(new pmt_f64vector(k, &data[0]));
    }
    return pmt_t(
        new pmt_f64vector(k, static_cast<double>(0))); // fills an empty vector with 0
}

double f64vector_ref(pmt_t vector, size_t k)
{
    if (!vector->is_f64vector())
        throw wrong_type("pmt_f64vector_ref", vector);
    return _f64vector(vector)->ref(k);
}

void f64vector_set(pmt_t vector, size_t k, double obj)
{
    if (!vector->is_f64vector())
        throw wrong_type("pmt_f64vector_set", vector);
    _f64vector(vector)->set(k, obj);
}

const double* f64vector_elements(pmt_t vector, size_t& len)
{
    if (!vector->is_f64vector())
        throw wrong_type("pmt_f64vector_elements", vector);
    return _f64vector(vector)->elements(len);
}

const std::vector<double> f64vector_elements(pmt_t vector)
{
    if (!vector->is_f64vector())
        throw wrong_type("pmt_f64vector_elements", vector);
    size_t len;
    const double* array = _f64vector(vector)->elements(len);
    const std::vector<double> vec(array, array + len);
    return vec;
}


double* f64vector_writable_elements(pmt_t vector, size_t& len)
{
    if (!vector->is_f64vector())
        throw wrong_type("pmt_f64vector_writable_elements", vector);
    return _f64vector(vector)->writable_elements(len);
}

const std::string pmt_f64vector::string_ref(size_t k) const
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(std::numeric_limits<double>::digits10)
       << ref(k);
    return ss.str();
}

} /* namespace pmt */
////////////////////////////////////////////////////////////////////////////
//                           pmt_c32vector
////////////////////////////////////////////////////////////////////////////

namespace pmt {

static pmt_c32vector* _c32vector(pmt_t x)
{
    return dynamic_cast<pmt_c32vector*>(x.get());
}


pmt_c32vector::pmt_c32vector(size_t k, std::complex<float> fill) : d_v(k)
{
    for (size_t i = 0; i < k; i++)
        d_v[i] = fill;
}

pmt_c32vector::pmt_c32vector(size_t k, const std::complex<float>* data) : d_v(k)
{
    if (k)
        std::memcpy(d_v.data(), data, k * sizeof(std::complex<float>));
}

std::complex<float> pmt_c32vector::ref(size_t k) const
{
    if (k >= length())
        throw out_of_range("pmt_c32vector_ref", from_long(k));
    return d_v[k];
}

void pmt_c32vector::set(size_t k, std::complex<float> x)
{
    if (k >= length())
        throw out_of_range("pmt_c32vector_set", from_long(k));
    d_v[k] = x;
}

const std::complex<float>* pmt_c32vector::elements(size_t& len)
{
    len = length();
    return len ? d_v.data() : nullptr;
}

std::complex<float>* pmt_c32vector::writable_elements(size_t& len)
{
    len = length();
    return len ? d_v.data() : nullptr;
}

const void* pmt_c32vector::uniform_elements(size_t& len)
{
    len = length() * sizeof(std::complex<float>);
    return len ? d_v.data() : nullptr;
}

void* pmt_c32vector::uniform_writable_elements(size_t& len)
{
    len = length() * sizeof(std::complex<float>);
    return len ? (d_v.data()) : nullptr;
}

bool is_c32vector(pmt_t obj) { return obj->is_c32vector(); }

pmt_t make_c32vector(size_t k, std::complex<float> fill)
{
    return pmt_t(new pmt_c32vector(k, fill));
}

pmt_t init_c32vector(size_t k, const std::complex<float>* data)
{
    return pmt_t(new pmt_c32vector(k, data));
}

pmt_t init_c32vector(size_t k, const std::vector<std::complex<float>>& data)
{
    if (k) {
        return pmt_t(new pmt_c32vector(k, &data[0]));
    }
    return pmt_t(new pmt_c32vector(
        k, static_cast<std::complex<float>>(0))); // fills an empty vector with 0
}

std::complex<float> c32vector_ref(pmt_t vector, size_t k)
{
    if (!vector->is_c32vector())
        throw wrong_type("pmt_c32vector_ref", vector);
    return _c32vector(vector)->ref(k);
}

void c32vector_set(pmt_t vector, size_t k, std::complex<float> obj)
{
    if (!vector->is_c32vector())
        throw wrong_type("pmt_c32vector_set", vector);
    _c32vector(vector)->set(k, obj);
}

const std::complex<float>* c32vector_elements(pmt_t vector, size_t& len)
{
    if (!vector->is_c32vector())
        throw wrong_type("pmt_c32vector_elements", vector);
    return _c32vector(vector)->elements(len);
}

const std::vector<std::complex<float>> c32vector_elements(pmt_t vector)
{
    if (!vector->is_c32vector())
        throw wrong_type("pmt_c32vector_elements", vector);
    size_t len;
    const std::complex<float>* array = _c32vector(vector)->elements(len);
    const std::vector<std::complex<float>> vec(array, array + len);
    return vec;
}


std::complex<float>* c32vector_writable_elements(pmt_t vector, size_t& len)
{
    if (!vector->is_c32vector())
        throw wrong_type("pmt_c32vector_writable_elements", vector);
    return _c32vector(vector)->writable_elements(len);
}

const std::string pmt_c32vector::string_ref(size_t k) const
{
    std::stringstream ss;
    ss << ref(k);
    return ss.str();
}

} /* namespace pmt */
////////////////////////////////////////////////////////////////////////////
//                           pmt_c64vector
////////////////////////////////////////////////////////////////////////////

namespace pmt {

static pmt_c64vector* _c64vector(pmt_t x)
{
    return dynamic_cast<pmt_c64vector*>(x.get());
}


pmt_c64vector::pmt_c64vector(size_t k, std::complex<double> fill) : d_v(k)
{
    for (size_t i = 0; i < k; i++)
        d_v[i] = fill;
}

pmt_c64vector::pmt_c64vector(size_t k, const std::complex<double>* data) : d_v(k)
{
    if (k)
        std::memcpy(d_v.data(), data, k * sizeof(std::complex<double>));
}

std::complex<double> pmt_c64vector::ref(size_t k) const
{
    if (k >= length())
        throw out_of_range("pmt_c64vector_ref", from_long(k));
    return d_v[k];
}

void pmt_c64vector::set(size_t k, std::complex<double> x)
{
    if (k >= length())
        throw out_of_range("pmt_c64vector_set", from_long(k));
    d_v[k] = x;
}

const std::complex<double>* pmt_c64vector::elements(size_t& len)
{
    len = length();
    return len ? d_v.data() : nullptr;
}

std::complex<double>* pmt_c64vector::writable_elements(size_t& len)
{
    len = length();
    return len ? d_v.data() : nullptr;
}

const void* pmt_c64vector::uniform_elements(size_t& len)
{
    len = length() * sizeof(std::complex<double>);
    return len ? d_v.data() : nullptr;
}

void* pmt_c64vector::uniform_writable_elements(size_t& len)
{
    len = length() * sizeof(std::complex<double>);
    return len ? (d_v.data()) : nullptr;
}

bool is_c64vector(pmt_t obj) { return obj->is_c64vector(); }

pmt_t make_c64vector(size_t k, std::complex<double> fill)
{
    return pmt_t(new pmt_c64vector(k, fill));
}

pmt_t init_c64vector(size_t k, const std::complex<double>* data)
{
    return pmt_t(new pmt_c64vector(k, data));
}

pmt_t init_c64vector(size_t k, const std::vector<std::complex<double>>& data)
{
    if (k) {
        return pmt_t(new pmt_c64vector(k, &data[0]));
    }
    return pmt_t(new pmt_c64vector(
        k, static_cast<std::complex<double>>(0))); // fills an empty vector with 0
}

std::complex<double> c64vector_ref(pmt_t vector, size_t k)
{
    if (!vector->is_c64vector())
        throw wrong_type("pmt_c64vector_ref", vector);
    return _c64vector(vector)->ref(k);
}

void c64vector_set(pmt_t vector, size_t k, std::complex<double> obj)
{
    if (!vector->is_c64vector())
        throw wrong_type("pmt_c64vector_set", vector);
    _c64vector(vector)->set(k, obj);
}

const std::complex<double>* c64vector_elements(pmt_t vector, size_t& len)
{
    if (!vector->is_c64vector())
        throw wrong_type("pmt_c64vector_elements", vector);
    return _c64vector(vector)->elements(len);
}

const std::vector<std::complex<double>> c64vector_elements(pmt_t vector)
{
    if (!vector->is_c64vector())
        throw wrong_type("pmt_c64vector_elements", vector);
    size_t len;
    const std::complex<double>* array = _c64vector(vector)->elements(len);
    const std::vector<std::complex<double>> vec(array, array + len);
    return vec;
}


std::complex<double>* c64vector_writable_elements(pmt_t vector, size_t& len)
{
    if (!vector->is_c64vector())
        throw wrong_type("pmt_c64vector_writable_elements", vector);
    return _c64vector(vector)->writable_elements(len);
}

const std::string pmt_c64vector::string_ref(size_t k) const
{
    std::stringstream ss;
    ss << ref(k);
    return ss.str();
}

} /* namespace pmt */
