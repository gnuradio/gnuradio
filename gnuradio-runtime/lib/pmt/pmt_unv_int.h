/* -*- c++ -*- */
/*
 * Copyright 2006,2009,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_PMT_UNV_INT_H
#define INCLUDED_PMT_UNV_INT_H

#include "pmt_int.h"

#include <volk/volk_alloc.hh>

#include <cstdint>
#include <vector>

namespace pmt {
////////////////////////////////////////////////////////////////////////////
//                           pmt_u8vector
////////////////////////////////////////////////////////////////////////////
class PMT_API pmt_u8vector : public pmt_uniform_vector
{
    volk::vector<uint8_t> d_v;

public:
    pmt_u8vector(size_t k, uint8_t fill);
    pmt_u8vector(size_t k, const uint8_t* data);
    // ~pmt_u8vector();

    bool is_u8vector() const override { return true; }
    size_t length() const override { return d_v.size(); }
    size_t itemsize() const override { return sizeof(uint8_t); }
    uint8_t ref(size_t k) const;
    void set(size_t k, uint8_t x);
    const uint8_t* elements(size_t& len);
    uint8_t* writable_elements(size_t& len);
    const void* uniform_elements(size_t& len) override;
    void* uniform_writable_elements(size_t& len) override;
    const std::string string_ref(size_t k) const override;
};

////////////////////////////////////////////////////////////////////////////
//                           pmt_s8vector
////////////////////////////////////////////////////////////////////////////

class pmt_s8vector : public pmt_uniform_vector
{
    volk::vector<int8_t> d_v;

public:
    pmt_s8vector(size_t k, int8_t fill);
    pmt_s8vector(size_t k, const int8_t* data);
    // ~pmt_s8vector();

    bool is_s8vector() const override { return true; }
    size_t length() const override { return d_v.size(); }
    size_t itemsize() const override { return sizeof(int8_t); }
    int8_t ref(size_t k) const;
    void set(size_t k, int8_t x);
    const int8_t* elements(size_t& len);
    int8_t* writable_elements(size_t& len);
    const void* uniform_elements(size_t& len) override;
    void* uniform_writable_elements(size_t& len) override;
    const std::string string_ref(size_t k) const override;
};

////////////////////////////////////////////////////////////////////////////
//                           pmt_u16vector
////////////////////////////////////////////////////////////////////////////

class pmt_u16vector : public pmt_uniform_vector
{
    volk::vector<uint16_t> d_v;

public:
    pmt_u16vector(size_t k, uint16_t fill);
    pmt_u16vector(size_t k, const uint16_t* data);
    // ~pmt_u16vector();

    bool is_u16vector() const override { return true; }
    size_t length() const override { return d_v.size(); }
    size_t itemsize() const override { return sizeof(uint16_t); }
    uint16_t ref(size_t k) const;
    void set(size_t k, uint16_t x);
    const uint16_t* elements(size_t& len);
    uint16_t* writable_elements(size_t& len);
    const void* uniform_elements(size_t& len) override;
    void* uniform_writable_elements(size_t& len) override;
    const std::string string_ref(size_t k) const override;
};

////////////////////////////////////////////////////////////////////////////
//                           pmt_s16vector
////////////////////////////////////////////////////////////////////////////

class pmt_s16vector : public pmt_uniform_vector
{
    volk::vector<int16_t> d_v;

public:
    pmt_s16vector(size_t k, int16_t fill);
    pmt_s16vector(size_t k, const int16_t* data);
    // ~pmt_s16vector();

    bool is_s16vector() const override { return true; }
    size_t length() const override { return d_v.size(); }
    size_t itemsize() const override { return sizeof(int16_t); }
    int16_t ref(size_t k) const;
    void set(size_t k, int16_t x);
    const int16_t* elements(size_t& len);
    int16_t* writable_elements(size_t& len);
    const void* uniform_elements(size_t& len) override;
    void* uniform_writable_elements(size_t& len) override;
    const std::string string_ref(size_t k) const override;
};

////////////////////////////////////////////////////////////////////////////
//                           pmt_u32vector
////////////////////////////////////////////////////////////////////////////

class pmt_u32vector : public pmt_uniform_vector
{
    volk::vector<uint32_t> d_v;

public:
    pmt_u32vector(size_t k, uint32_t fill);
    pmt_u32vector(size_t k, const uint32_t* data);
    // ~pmt_u32vector();

    bool is_u32vector() const override { return true; }
    size_t length() const override { return d_v.size(); }
    size_t itemsize() const override { return sizeof(uint32_t); }
    uint32_t ref(size_t k) const;
    void set(size_t k, uint32_t x);
    const uint32_t* elements(size_t& len);
    uint32_t* writable_elements(size_t& len);
    const void* uniform_elements(size_t& len) override;
    void* uniform_writable_elements(size_t& len) override;
    const std::string string_ref(size_t k) const override;
};

////////////////////////////////////////////////////////////////////////////
//                           pmt_s32vector
////////////////////////////////////////////////////////////////////////////

class pmt_s32vector : public pmt_uniform_vector
{
    volk::vector<int32_t> d_v;

public:
    pmt_s32vector(size_t k, int32_t fill);
    pmt_s32vector(size_t k, const int32_t* data);
    // ~pmt_s32vector();

    bool is_s32vector() const override { return true; }
    size_t length() const override { return d_v.size(); }
    size_t itemsize() const override { return sizeof(int32_t); }
    int32_t ref(size_t k) const;
    void set(size_t k, int32_t x);
    const int32_t* elements(size_t& len);
    int32_t* writable_elements(size_t& len);
    const void* uniform_elements(size_t& len) override;
    void* uniform_writable_elements(size_t& len) override;
    const std::string string_ref(size_t k) const override;
};

////////////////////////////////////////////////////////////////////////////
//                           pmt_u64vector
////////////////////////////////////////////////////////////////////////////

class pmt_u64vector : public pmt_uniform_vector
{
    volk::vector<uint64_t> d_v;

public:
    pmt_u64vector(size_t k, uint64_t fill);
    pmt_u64vector(size_t k, const uint64_t* data);
    // ~pmt_u64vector();

    bool is_u64vector() const override { return true; }
    size_t length() const override { return d_v.size(); }
    size_t itemsize() const override { return sizeof(uint64_t); }
    uint64_t ref(size_t k) const;
    void set(size_t k, uint64_t x);
    const uint64_t* elements(size_t& len);
    uint64_t* writable_elements(size_t& len);
    const void* uniform_elements(size_t& len) override;
    void* uniform_writable_elements(size_t& len) override;
    const std::string string_ref(size_t k) const override;
};

////////////////////////////////////////////////////////////////////////////
//                           pmt_s64vector
////////////////////////////////////////////////////////////////////////////

class pmt_s64vector : public pmt_uniform_vector
{
    volk::vector<int64_t> d_v;

public:
    pmt_s64vector(size_t k, int64_t fill);
    pmt_s64vector(size_t k, const int64_t* data);
    // ~pmt_s64vector();

    bool is_s64vector() const override { return true; }
    size_t length() const override { return d_v.size(); }
    size_t itemsize() const override { return sizeof(int64_t); }
    int64_t ref(size_t k) const;
    void set(size_t k, int64_t x);
    const int64_t* elements(size_t& len);
    int64_t* writable_elements(size_t& len);
    const void* uniform_elements(size_t& len) override;
    void* uniform_writable_elements(size_t& len) override;
    const std::string string_ref(size_t k) const override;
};

////////////////////////////////////////////////////////////////////////////
//                           pmt_f32vector
////////////////////////////////////////////////////////////////////////////

class pmt_f32vector : public pmt_uniform_vector
{
    volk::vector<float> d_v;

public:
    pmt_f32vector(size_t k, float fill);
    pmt_f32vector(size_t k, const float* data);
    // ~pmt_f32vector();

    bool is_f32vector() const override { return true; }
    size_t length() const override { return d_v.size(); }
    size_t itemsize() const override { return sizeof(float); }
    float ref(size_t k) const;
    void set(size_t k, float x);
    const float* elements(size_t& len);
    float* writable_elements(size_t& len);
    const void* uniform_elements(size_t& len) override;
    void* uniform_writable_elements(size_t& len) override;
    const std::string string_ref(size_t k) const override;
};

////////////////////////////////////////////////////////////////////////////
//                           pmt_f64vector
////////////////////////////////////////////////////////////////////////////

class pmt_f64vector : public pmt_uniform_vector
{
    volk::vector<double> d_v;

public:
    pmt_f64vector(size_t k, double fill);
    pmt_f64vector(size_t k, const double* data);
    // ~pmt_f64vector();

    bool is_f64vector() const override { return true; }
    size_t length() const override { return d_v.size(); }
    size_t itemsize() const override { return sizeof(double); }
    double ref(size_t k) const;
    void set(size_t k, double x);
    const double* elements(size_t& len);
    double* writable_elements(size_t& len);
    const void* uniform_elements(size_t& len) override;
    void* uniform_writable_elements(size_t& len) override;
    const std::string string_ref(size_t k) const override;
};

////////////////////////////////////////////////////////////////////////////
//                           pmt_c32vector
////////////////////////////////////////////////////////////////////////////

class pmt_c32vector : public pmt_uniform_vector
{
    volk::vector<std::complex<float>> d_v;

public:
    pmt_c32vector(size_t k, std::complex<float> fill);
    pmt_c32vector(size_t k, const std::complex<float>* data);
    // ~pmt_c32vector();

    bool is_c32vector() const override { return true; }
    size_t length() const override { return d_v.size(); }
    size_t itemsize() const override { return sizeof(std::complex<float>); }
    std::complex<float> ref(size_t k) const;
    void set(size_t k, std::complex<float> x);
    const std::complex<float>* elements(size_t& len);
    std::complex<float>* writable_elements(size_t& len);
    const void* uniform_elements(size_t& len) override;
    void* uniform_writable_elements(size_t& len) override;
    const std::string string_ref(size_t k) const override;
};

////////////////////////////////////////////////////////////////////////////
//                           pmt_c64vector
////////////////////////////////////////////////////////////////////////////

class pmt_c64vector : public pmt_uniform_vector
{
    volk::vector<std::complex<double>> d_v;

public:
    pmt_c64vector(size_t k, std::complex<double> fill);
    pmt_c64vector(size_t k, const std::complex<double>* data);
    // ~pmt_c64vector();

    bool is_c64vector() const override { return true; }
    size_t length() const override { return d_v.size(); }
    size_t itemsize() const override { return sizeof(std::complex<double>); }
    std::complex<double> ref(size_t k) const;
    void set(size_t k, std::complex<double> x);
    const std::complex<double>* elements(size_t& len);
    std::complex<double>* writable_elements(size_t& len);
    const void* uniform_elements(size_t& len) override;
    void* uniform_writable_elements(size_t& len) override;
    const std::string string_ref(size_t k) const override;
};
} /* namespace pmt */
#endif
