/* -*- c++ -*- */
/*
 * Copyright 2007,2009 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "pmt/pmt_serial_tags.h"
#include "pmt_int.h"
#include <pmt/pmt.h>
#include <cstdint>
#include <limits>
#include <vector>

namespace pmt {

static pmt_t parse_pair(std::streambuf& sb, uint8_t type);

// ----------------------------------------------------------------
// output primitives
// ----------------------------------------------------------------

static inline void native_to_big_u16(uint16_t i, uint8_t* buf)
{
    buf[0] = i >> 8;
    buf[1] = i & 0xff;
}

static inline void native_to_big_u32(uint32_t i, uint8_t* buf)
{
    buf[0] = i >> 24;
    buf[1] = (i >> 16) & 0xff;
    buf[2] = (i >> 8) & 0xff;
    buf[3] = i & 0xff;
}

static inline void native_to_big_u64(uint64_t i, uint8_t* buf)
{
    buf[0] = i >> 56;
    buf[1] = (i >> 48) & 0xff;
    buf[2] = (i >> 40) & 0xff;
    buf[3] = (i >> 32) & 0xff;
    buf[4] = (i >> 24) & 0xff;
    buf[5] = (i >> 16) & 0xff;
    buf[6] = (i >> 8) & 0xff;
    buf[7] = i & 0xff;
}

static bool serialize_untagged_u8(uint8_t i, std::streambuf& sb)
{
    return sb.sputc(i) != std::streambuf::traits_type::eof();
}

// always writes big-endian
static bool serialize_untagged_u16(uint16_t i, std::streambuf& sb)
{
    uint8_t buf[sizeof(uint16_t)];
    native_to_big_u16(i, buf);
    return sb.sputn((char*)buf, sizeof(buf)) != std::streambuf::traits_type::eof();
}

// always writes big-endian
static bool serialize_untagged_u32(uint32_t i, std::streambuf& sb)
{
    uint8_t buf[sizeof(uint32_t)];
    native_to_big_u32(i, buf);
    return sb.sputn((char*)buf, sizeof(buf)) != std::streambuf::traits_type::eof();
}

static bool serialize_untagged_f64(double i, std::streambuf& sb)
{
    typedef union {
        double id;
        uint64_t ii;
    } iu_t;
    iu_t iu;
    iu.id = i;
    uint8_t buf[sizeof(uint64_t)];
    native_to_big_u64(iu.ii, buf);
    return sb.sputn((char*)buf, sizeof(buf)) != std::streambuf::traits_type::eof();
}

// always writes big-endian
static bool serialize_untagged_u64(uint64_t i, std::streambuf& sb)
{
    uint8_t buf[sizeof(uint64_t)];
    native_to_big_u64(i, buf);
    return sb.sputn((char*)buf, sizeof(buf)) != std::streambuf::traits_type::eof();
}

// always writes big-endian
static bool
serialize_untagged_u8_array(const uint8_t* data, size_t length, std::streambuf& sb)
{
    return sb.sputn((const char*)data, length) != std::streambuf::traits_type::eof();
}

static bool
serialize_untagged_u16_array(const uint16_t* data, size_t length, std::streambuf& sb)
{
    std::vector<uint8_t> bedata(length * sizeof(uint16_t));
    for (size_t i = 0; i < length; i++) {
        native_to_big_u16(data[i], &bedata[sizeof(uint16_t) * i]);
    }
    return sb.sputn((const char*)&bedata[0], length * sizeof(uint16_t)) !=
           std::streambuf::traits_type::eof();
}

static bool
serialize_untagged_u32_array(const uint32_t* data, size_t length, std::streambuf& sb)
{
    std::vector<uint8_t> bedata(length * sizeof(uint32_t));
    for (size_t i = 0; i < length; i++) {
        native_to_big_u32(data[i], &bedata[sizeof(uint32_t) * i]);
    }
    return sb.sputn((const char*)&bedata[0], length * sizeof(uint32_t)) !=
           std::streambuf::traits_type::eof();
}

static bool
serialize_untagged_u64_array(const uint64_t* data, size_t length, std::streambuf& sb)
{
    std::vector<uint8_t> bedata(length * sizeof(uint64_t));
    for (size_t i = 0; i < length; i++) {
        native_to_big_u64(data[i], &bedata[sizeof(uint64_t) * i]);
    }
    return sb.sputn((const char*)&bedata[0], length * sizeof(uint64_t)) !=
           std::streambuf::traits_type::eof();
}

// ----------------------------------------------------------------
// input primitives
// ----------------------------------------------------------------

static inline uint32_t big_to_native_u16(uint8_t* buf) { return (buf[0] << 8) | buf[1]; }

static inline uint32_t big_to_native_u32(uint8_t* buf)
{
    return (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}

static inline uint64_t big_to_native_u64(uint8_t* buf)
{
    return ((uint64_t)buf[0] << 56) | ((uint64_t)buf[1] << 48) |
           ((uint64_t)buf[2] << 40) | ((uint64_t)buf[3] << 32) |
           ((uint64_t)buf[4] << 24) | ((uint64_t)buf[5] << 16) | ((uint64_t)buf[6] << 8) |
           (uint64_t)buf[7];
}

// always reads big-endian
static bool deserialize_untagged_u8(uint8_t* ip, std::streambuf& sb)
{
    std::streambuf::traits_type::int_type t;
    int i;

    t = sb.sbumpc();
    i = t & 0xff;

    *ip = i;
    return t != std::streambuf::traits_type::eof();
}

// always reads big-endian
static bool deserialize_untagged_u16(uint16_t* ip, std::streambuf& sb)
{
    uint8_t buf[sizeof(uint16_t)];
    std::streambuf::traits_type::int_type t;
    t = sb.sgetn((char*)buf, sizeof(buf));
    *ip = big_to_native_u16(buf);

    return t != std::streambuf::traits_type::eof();
}

// always reads big-endian
static bool deserialize_untagged_u32(uint32_t* ip, std::streambuf& sb)
{
    uint8_t buf[sizeof(uint32_t)];
    std::streambuf::traits_type::int_type t;
    t = sb.sgetn((char*)buf, sizeof(buf));
    *ip = big_to_native_u32(buf);

    return t != std::streambuf::traits_type::eof();
}

// always reads big-endian
static bool deserialize_untagged_u64(uint64_t* ip, std::streambuf& sb)
{
    uint8_t buf[sizeof(uint64_t)];
    std::streambuf::traits_type::int_type t;
    t = sb.sgetn((char*)buf, sizeof(buf));
    *ip = big_to_native_u64(buf);

    return t != std::streambuf::traits_type::eof();
}

static bool deserialize_untagged_f64(double* ip, std::streambuf& sb)
{
    uint8_t buf[sizeof(uint64_t)];
    std::streambuf::traits_type::int_type t;

    typedef union {
        double id;
        uint64_t ii;
    } iu_t;

    iu_t iu;
    t = sb.sgetn((char*)buf, sizeof(buf));
    iu.ii = big_to_native_u64(buf);
    *ip = iu.id;

    return t != std::streambuf::traits_type::eof();
}

static bool deserialize_tuple(pmt_t* tuple, std::streambuf& sb)
{
    uint32_t nitems;
    bool ok = deserialize_untagged_u32(&nitems, sb);
    pmt_t list(PMT_NIL);
    for (uint32_t i = 0; i < nitems; i++) {
        pmt_t item = deserialize(sb);
        if (eq(list, PMT_NIL)) {
            list = list1(item);
        } else {
            list = list_add(list, item);
        }
    }
    (*tuple) = to_tuple(list);
    return ok;
}

// always reads big-endian
static bool deserialize_untagged_u8_vector(std::vector<uint8_t>& data,
                                           size_t nitems,
                                           std::streambuf& sb)
{
    std::streambuf::traits_type::int_type t;
    data.resize(nitems);
    t = sb.sgetn((char*)&data[0], nitems);

    return t != std::streambuf::traits_type::eof();
}

static bool deserialize_untagged_u16_vector(std::vector<uint16_t>& data,
                                            size_t nitems,
                                            std::streambuf& sb)
{
    std::streambuf::traits_type::int_type t;
    data.resize(nitems);
    std::vector<uint8_t> buf(nitems * sizeof(uint16_t));
    t = sb.sgetn((char*)&buf[0], nitems * sizeof(uint16_t));
    for (size_t i = 0; i < nitems; i++) {
        data[i] = big_to_native_u16(&buf[sizeof(uint16_t) * i]);
    }

    return t != std::streambuf::traits_type::eof();
}

static bool deserialize_untagged_u32_vector(std::vector<uint32_t>& data,
                                            size_t nitems,
                                            std::streambuf& sb)
{
    std::streambuf::traits_type::int_type t;
    data.resize(nitems);
    std::vector<uint8_t> buf(nitems * sizeof(uint32_t));
    t = sb.sgetn((char*)&buf[0], nitems * sizeof(uint32_t));
    for (size_t i = 0; i < nitems; i++) {
        data[i] = big_to_native_u32(&buf[sizeof(uint32_t) * i]);
    }

    return t != std::streambuf::traits_type::eof();
}

static bool deserialize_untagged_u64_vector(std::vector<uint64_t>& data,
                                            size_t nitems,
                                            std::streambuf& sb)
{
    std::streambuf::traits_type::int_type t;
    data.resize(nitems);
    std::vector<uint8_t> buf(nitems * sizeof(uint64_t));
    t = sb.sgetn((char*)&buf[0], nitems * sizeof(uint64_t));
    for (size_t i = 0; i < nitems; i++) {
        data[i] = big_to_native_u64(&buf[sizeof(uint64_t) * i]);
    }

    return t != std::streambuf::traits_type::eof();
}

/*
 * Write portable byte-serial representation of \p obj to \p sb
 *
 * N.B., Circular structures cause infinite recursion.
 */
bool serialize(pmt_t obj, std::streambuf& sb)
{
    bool ok = true;

tail_recursion:

    if (is_bool(obj)) {
        if (eq(obj, PMT_T))
            return serialize_untagged_u8(PST_TRUE, sb);
        else
            return serialize_untagged_u8(PST_FALSE, sb);
    }

    if (is_null(obj))
        return serialize_untagged_u8(PST_NULL, sb);

    if (is_symbol(obj)) {
        const std::string s = symbol_to_string(obj);
        size_t len = s.size();
        ok = serialize_untagged_u8(PST_SYMBOL, sb);
        ok &= serialize_untagged_u16(len, sb);
        ok &= serialize_untagged_u8_array((const uint8_t*)s.c_str(), len, sb);
        return ok;
    }

    if (is_pair(obj)) {
        ok = serialize_untagged_u8(is_dict(obj) ? PST_DICT : PST_PAIR, sb);
        ok &= serialize(car(obj), sb);
        if (!ok)
            return false;
        obj = cdr(obj);
        goto tail_recursion;
    }

    if (is_number(obj)) {

        if (is_uint64(obj)) {
            uint64_t i = to_uint64(obj);
            ok = serialize_untagged_u8(PST_UINT64, sb);
            ok &= serialize_untagged_u64(i, sb);
            return ok;
        } else {
            if (is_integer(obj)) {
                long i = to_long(obj);
                if ((sizeof(long) > 4) &&
                    ((i < std::numeric_limits<std::int32_t>::min() ||
                      i > std::numeric_limits<std::int32_t>::max()))) {
                    // Serializing as 4 bytes won't work for this value, serialize as 8
                    // bytes
                    ok = serialize_untagged_u8(PST_INT64, sb);
                    ok &= serialize_untagged_u64(i, sb);
                } else {
                    ok = serialize_untagged_u8(PST_INT32, sb);
                    ok &= serialize_untagged_u32(i, sb);
                }
                return ok;
            }
        }

        if (is_real(obj)) {
            double i = to_double(obj);
            ok = serialize_untagged_u8(PST_DOUBLE, sb);
            ok &= serialize_untagged_f64(i, sb);
            return ok;
        }

        if (is_complex(obj)) {
            std::complex<double> i = to_complex(obj);
            ok = serialize_untagged_u8(PST_COMPLEX, sb);
            ok &= serialize_untagged_f64(i.real(), sb);
            ok &= serialize_untagged_f64(i.imag(), sb);
            return ok;
        }
    }

    if (is_vector(obj)) {
        size_t vec_len = pmt::length(obj);
        ok = serialize_untagged_u8(PST_VECTOR, sb);
        ok &= serialize_untagged_u32(vec_len, sb);
        for (size_t i = 0; i < vec_len; i++) {
            ok &= serialize(vector_ref(obj, i), sb);
        }
        return ok;
    }

    if (is_uniform_vector(obj)) {
        size_t npad = 1;
        size_t vec_len = pmt::length(obj);

        if (is_u8vector(obj)) {
            ok = serialize_untagged_u8(PST_UNIFORM_VECTOR, sb);
            ok &= serialize_untagged_u8(UVI_U8, sb);
            ok &= serialize_untagged_u32(vec_len, sb);
            ok &= serialize_untagged_u8(npad, sb);
            for (size_t i = 0; i < npad; i++) {
                ok &= serialize_untagged_u8(0, sb);
            }
            ok &= serialize_untagged_u8_array(&u8vector_elements(obj)[0], vec_len, sb);
            return ok;
        }

        if (is_s8vector(obj)) {
            ok = serialize_untagged_u8(PST_UNIFORM_VECTOR, sb);
            ok &= serialize_untagged_u8(UVI_S8, sb);
            ok &= serialize_untagged_u32(vec_len, sb);
            ok &= serialize_untagged_u8(npad, sb);
            for (size_t i = 0; i < npad; i++) {
                ok &= serialize_untagged_u8(0, sb);
            }
            ok &= serialize_untagged_u8_array(
                (const uint8_t*)&s8vector_elements(obj)[0], vec_len, sb);

            return ok;
        }

        if (is_u16vector(obj)) {
            ok = serialize_untagged_u8(PST_UNIFORM_VECTOR, sb);
            ok &= serialize_untagged_u8(UVI_U16, sb);
            ok &= serialize_untagged_u32(vec_len, sb);
            ok &= serialize_untagged_u8(npad, sb);
            for (size_t i = 0; i < npad; i++) {
                ok &= serialize_untagged_u8(0, sb);
            }
            ok &= serialize_untagged_u16_array(&u16vector_elements(obj)[0], vec_len, sb);
            return ok;
        }

        if (is_s16vector(obj)) {
            ok = serialize_untagged_u8(PST_UNIFORM_VECTOR, sb);
            ok &= serialize_untagged_u8(UVI_S16, sb);
            ok &= serialize_untagged_u32(vec_len, sb);
            ok &= serialize_untagged_u8(npad, sb);
            for (size_t i = 0; i < npad; i++) {
                ok &= serialize_untagged_u8(0, sb);
            }
            ok &= serialize_untagged_u16_array(
                (const uint16_t*)&s16vector_elements(obj)[0], vec_len, sb);
            return ok;
        }

        if (is_u32vector(obj)) {
            ok = serialize_untagged_u8(PST_UNIFORM_VECTOR, sb);
            ok &= serialize_untagged_u8(UVI_U32, sb);
            ok &= serialize_untagged_u32(vec_len, sb);
            ok &= serialize_untagged_u8(npad, sb);
            for (size_t i = 0; i < npad; i++) {
                ok &= serialize_untagged_u8(0, sb);
            }
            ok &= serialize_untagged_u32_array(&u32vector_elements(obj)[0], vec_len, sb);
            return ok;
        }

        if (is_s32vector(obj)) {
            ok = serialize_untagged_u8(PST_UNIFORM_VECTOR, sb);
            ok &= serialize_untagged_u8(UVI_S32, sb);
            ok &= serialize_untagged_u32(vec_len, sb);
            ok &= serialize_untagged_u8(npad, sb);
            for (size_t i = 0; i < npad; i++) {
                ok &= serialize_untagged_u8(0, sb);
            }
            ok &= serialize_untagged_u32_array(
                (const uint32_t*)&s32vector_elements(obj)[0], vec_len, sb);
            return ok;
        }

        if (is_u64vector(obj)) {
            ok = serialize_untagged_u8(PST_UNIFORM_VECTOR, sb);
            ok &= serialize_untagged_u8(UVI_U64, sb);
            ok &= serialize_untagged_u32(vec_len, sb);
            ok &= serialize_untagged_u8(npad, sb);
            for (size_t i = 0; i < npad; i++) {
                ok &= serialize_untagged_u8(0, sb);
            }
            ok &= serialize_untagged_u64_array(&u64vector_elements(obj)[0], vec_len, sb);
            return ok;
        }

        if (is_s64vector(obj)) {
            ok = serialize_untagged_u8(PST_UNIFORM_VECTOR, sb);
            ok &= serialize_untagged_u8(UVI_S64, sb);
            ok &= serialize_untagged_u32(vec_len, sb);
            ok &= serialize_untagged_u8(npad, sb);
            for (size_t i = 0; i < npad; i++) {
                ok &= serialize_untagged_u8(0, sb);
            }
            ok &= serialize_untagged_u64_array(
                (const uint64_t*)&s64vector_elements(obj)[0], vec_len, sb);
            return ok;
        }

        if (is_f32vector(obj)) {
            ok = serialize_untagged_u8(PST_UNIFORM_VECTOR, sb);
            ok &= serialize_untagged_u8(UVI_F32, sb);
            ok &= serialize_untagged_u32(vec_len, sb);
            ok &= serialize_untagged_u8(npad, sb);
            for (size_t i = 0; i < npad; i++) {
                ok &= serialize_untagged_u8(0, sb);
            }
            ok &= serialize_untagged_u32_array(
                (const uint32_t*)&f32vector_elements(obj)[0], vec_len, sb);
            return ok;
        }

        if (is_f64vector(obj)) {
            ok = serialize_untagged_u8(PST_UNIFORM_VECTOR, sb);
            ok &= serialize_untagged_u8(UVI_F64, sb);
            ok &= serialize_untagged_u32(vec_len, sb);
            ok &= serialize_untagged_u8(npad, sb);
            for (size_t i = 0; i < npad; i++) {
                ok &= serialize_untagged_u8(0, sb);
            }
            ok &= serialize_untagged_u64_array(
                (const uint64_t*)&f64vector_elements(obj)[0], vec_len, sb);
            return ok;
        }

        if (is_c32vector(obj)) {
            ok = serialize_untagged_u8(PST_UNIFORM_VECTOR, sb);
            ok &= serialize_untagged_u8(UVI_C32, sb);
            ok &= serialize_untagged_u32(vec_len, sb);
            ok &= serialize_untagged_u8(npad, sb);
            for (size_t i = 0; i < npad; i++) {
                ok &= serialize_untagged_u8(0, sb);
            }
            ok &= serialize_untagged_u32_array(
                (const uint32_t*)&c32vector_elements(obj)[0], vec_len * 2, sb);
            return ok;
        }

        if (is_c64vector(obj)) {
            ok = serialize_untagged_u8(PST_UNIFORM_VECTOR, sb);
            ok &= serialize_untagged_u8(UVI_C64, sb);
            ok &= serialize_untagged_u32(vec_len, sb);
            ok &= serialize_untagged_u8(npad, sb);
            for (size_t i = 0; i < npad; i++) {
                ok &= serialize_untagged_u8(0, sb);
            }
            // No known portable 128 bit swap function, so double the length
            ok &= serialize_untagged_u64_array(
                (const uint64_t*)&c64vector_elements(obj)[0], vec_len * 2, sb);
            return ok;
        }
    }

    if (is_dict(obj))
        throw notimplemented("pmt::serialize (dict)", obj);

    if (is_tuple(obj)) {
        size_t tuple_len = pmt::length(obj);
        ok = serialize_untagged_u8(PST_TUPLE, sb);
        ok &= serialize_untagged_u32(tuple_len, sb);
        for (size_t i = 0; i < tuple_len; i++) {
            ok &= serialize(tuple_ref(obj, i), sb);
        }
        return ok;
    }
    // throw pmt_notimplemented("pmt::serialize (tuple)", obj);

    throw notimplemented("pmt::serialize (?)", obj);
}

/*
 * Create obj from portable byte-serial representation
 *
 * Returns next obj from streambuf, or PMT_EOF at end of file.
 * Throws exception on malformed input.
 */
pmt_t deserialize(std::streambuf& sb)
{
    uint8_t tag;
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    double f64;
    char tmpbuf[1024];

    if (!deserialize_untagged_u8(&tag, sb))
        return PMT_EOF;

    switch (tag) {
    case PST_TRUE:
        return PMT_T;

    case PST_FALSE:
        return PMT_F;

    case PST_NULL:
        return PMT_NIL;

    case PST_SYMBOL:
        if (!deserialize_untagged_u16(&u16, sb))
            goto error;
        if (u16 > sizeof(tmpbuf))
            throw notimplemented("pmt::deserialize: very long symbol", PMT_F);
        if (sb.sgetn(tmpbuf, u16) != u16)
            goto error;
        return intern(std::string(tmpbuf, u16));

    case PST_INT32:
        if (!deserialize_untagged_u32(&u32, sb))
            goto error;
        return from_long((int32_t)u32);

    case PST_UINT64:
        if (!deserialize_untagged_u64(&u64, sb))
            goto error;
        return from_uint64(u64);

    case PST_INT64:
        if (!deserialize_untagged_u64(&u64, sb))
            goto error;
        return from_long(u64);

    case PST_PAIR:
        return parse_pair(sb, PST_PAIR);

    case PST_DICT:
        return parse_pair(sb, PST_DICT);

    case PST_DOUBLE:
        if (!deserialize_untagged_f64(&f64, sb))
            goto error;
        return from_double(f64);

    case PST_COMPLEX: {
        double r, i;
        if (!deserialize_untagged_f64(&r, sb) || !deserialize_untagged_f64(&i, sb))
            goto error;
        return make_rectangular(r, i);
    }

    case PST_TUPLE: {
        pmt_t tuple;
        if (!deserialize_tuple(&tuple, sb)) {
            goto error;
        }
        return tuple;
    }

    case PST_VECTOR: {
        uint32_t nitems;
        if (!deserialize_untagged_u32(&nitems, sb))
            goto error;
        pmt_t vec = make_vector(nitems, PMT_NIL);
        for (uint32_t i = 0; i < nitems; i++) {
            pmt_t item = deserialize(sb);
            vector_set(vec, i, item);
        }
        return vec;
    }

    case PST_UNIFORM_VECTOR: {
        uint8_t utag, npad;
        uint32_t nitems;
        std::vector<uint8_t> u8v;
        std::vector<uint16_t> u16v;
        std::vector<uint32_t> u32v;
        std::vector<uint64_t> u64v;

        if (!deserialize_untagged_u8(&utag, sb))
            return PMT_EOF;

        if (!deserialize_untagged_u32(&nitems, sb))
            goto error;

        deserialize_untagged_u8(&npad, sb);
        for (size_t i = 0; i < npad; i++)
            deserialize_untagged_u8(&u8, sb);


        switch (utag) {
        case (UVI_U8): {
            deserialize_untagged_u8_vector(u8v, nitems, sb);
            pmt_t vec = init_u8vector(nitems, &u8v[0]);
            return vec;
        }
        case (UVI_S8): {
            deserialize_untagged_u8_vector(u8v, nitems, sb);
            pmt_t vec = init_s8vector(nitems, (int8_t*)&u8v[0]);
            return vec;
        }
        case (UVI_U16): {
            deserialize_untagged_u16_vector(u16v, nitems, sb);
            pmt_t vec = init_u16vector(nitems, &u16v[0]);
            return vec;
        }
        case (UVI_S16): {
            deserialize_untagged_u16_vector(u16v, nitems, sb);
            pmt_t vec = init_s16vector(nitems, (int16_t*)&u16v[0]);
            return vec;
        }
        case (UVI_U32): {
            deserialize_untagged_u32_vector(u32v, nitems, sb);
            pmt_t vec = init_u32vector(nitems, &u32v[0]);
            return vec;
        }
        case (UVI_S32): {
            deserialize_untagged_u32_vector(u32v, nitems, sb);
            pmt_t vec = init_s32vector(nitems, (int32_t*)&u32v[0]);
            return vec;
        }
        case (UVI_U64): {
            deserialize_untagged_u64_vector(u64v, nitems, sb);
            pmt_t vec = init_u64vector(nitems, &u64v[0]);
            return vec;
        }
        case (UVI_S64): {
            deserialize_untagged_u64_vector(u64v, nitems, sb);
            pmt_t vec = init_s64vector(nitems, (int64_t*)&u64v[0]);
            return vec;
        }
        case (UVI_F32): {
            deserialize_untagged_u32_vector(u32v, nitems, sb);
            pmt_t vec = init_f32vector(nitems, (float*)&u32v[0]);
            return vec;
        }
        case (UVI_F64): {
            deserialize_untagged_u64_vector(u64v, nitems, sb);
            pmt_t vec = init_f64vector(nitems, (double*)&u64v[0]);
            return vec;
        }
        case (UVI_C32): {
            deserialize_untagged_u32_vector(u32v, 2 * nitems, sb);
            pmt_t vec = init_c32vector(nitems, (std::complex<float>*)&u32v[0]);
            return vec;
        }

        case (UVI_C64): {
            // Do 64 bits at a time, so we need to double length
            deserialize_untagged_u64_vector(u64v, 2 * nitems, sb);
            pmt_t vec = init_c64vector(nitems, (std::complex<double>*)&u64v[0]);
            return vec;
        }

        default:
            throw exception("pmt::deserialize: malformed input stream, tag value = ",
                            from_long(tag));
        }
    }

    case PST_COMMENT:
        throw notimplemented("pmt::deserialize: tag value = ", from_long(tag));

    default:
        throw exception("pmt::deserialize: malformed input stream, tag value = ",
                        from_long(tag));
    }

error:
    throw exception("pmt::deserialize: malformed input stream", PMT_F);
}

/*
 * provide a simple string accessor to the serialized pmt form
 */
std::string serialize_str(pmt_t obj)
{
    std::stringbuf sb;
    serialize(obj, sb);
    return sb.str();
}

/*
 * provide a simple string accessor to the deserialized pmt form
 */
pmt_t deserialize_str(std::string s)
{
    std::stringbuf sb(s);
    return deserialize(sb);
}

/*
 * This is a mostly non-recursive implementation that allows us to
 * deserialize very long lists w/o exhausting the evaluation stack.
 *
 * On entry we've already eaten the PST_PAIR or PST_DICT tag.
 */
pmt_t parse_pair(std::streambuf& sb, uint8_t type)
{
    uint8_t tag;
    pmt_t val, expr, lastnptr, nptr;

    //
    // Keep appending nodes until we get a non-PAIR cdr.
    //
    lastnptr = PMT_NIL;
    while (1) {
        expr = deserialize(sb); // read the car

        if (type == PST_DICT)
            nptr = dcons(expr, PMT_NIL); // build new cell
        else
            nptr = cons(expr, PMT_NIL); // build new cell

        if (is_null(lastnptr))
            val = nptr;
        else
            set_cdr(lastnptr, nptr);
        lastnptr = nptr;

        if (!deserialize_untagged_u8(&tag, sb)) // get tag of cdr
            throw exception("pmt::deserialize: malformed input stream", PMT_F);

        if (tag == PST_PAIR)
            continue; // keep on looping...

        if (tag == PST_NULL) {
            expr = PMT_NIL;
            break;
        }

        //
        // default: push tag back and use pmt_deserialize to get the cdr
        //
        sb.sungetc();
        expr = deserialize(sb);
        break;
    }

    //
    // At this point, expr contains the value of the final cdr in the list.
    //
    set_cdr(lastnptr, expr);
    return val;
}

} /* namespace pmt */
