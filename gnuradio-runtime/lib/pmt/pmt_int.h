/* -*- c++ -*- */
/*
 * Copyright 2006,2009,2010 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#ifndef INCLUDED_PMT_INT_H
#define INCLUDED_PMT_INT_H

#include <pmt/pmt.h>
#include <string_view>
#include <any>

/*
 * EVERYTHING IN THIS FILE IS PRIVATE TO THE IMPLEMENTATION!
 *
 * See pmt.h for the public interface
 */

namespace pmt {


class pmt_bool : public pmt_base
{
public:
    pmt_bool();
    //~pmt_bool(){}

    bool is_bool() const override { return true; }
};


class pmt_symbol : public pmt_base
{
    std::string d_name;
    pmt_t d_next;

public:
    pmt_symbol(std::string_view name);
    //~pmt_symbol(){}

    bool is_symbol() const override { return true; }
    const std::string name() { return d_name; }

    pmt_t next() { return d_next; } // symbol table link
    void set_next(pmt_t next) { d_next = next; }
};

class pmt_integer : public pmt_base
{
public:
    long d_value;

    pmt_integer(long value);
    //~pmt_integer(){}

    bool is_number() const override { return true; }
    bool is_integer() const override { return true; }
    long value() const { return d_value; }
};

class pmt_uint64 : public pmt_base
{
public:
    uint64_t d_value;

    pmt_uint64(uint64_t value);
    //~pmt_uint64(){}

    bool is_number() const override { return true; }
    bool is_uint64() const override { return true; }
    uint64_t value() const { return d_value; }
};

class pmt_real : public pmt_base
{
public:
    double d_value;

    pmt_real(double value);
    //~pmt_real(){}

    bool is_number() const override { return true; }
    bool is_real() const override { return true; }
    double value() const { return d_value; }
};

class pmt_complex : public pmt_base
{
public:
    std::complex<double> d_value;

    pmt_complex(std::complex<double> value);
    //~pmt_complex(){}

    bool is_number() const override { return true; }
    bool is_complex() const override { return true; }
    std::complex<double> value() const { return d_value; }
};

class pmt_null : public pmt_base
{
public:
    pmt_null();
    //~pmt_null(){}

    bool is_null() const override { return true; }
};

class pmt_pair : public pmt_base
{
public:
    pmt_t d_car;
    pmt_t d_cdr;

    pmt_pair(const pmt_t& car, const pmt_t& cdr);
    //~pmt_pair(){};

    bool is_pair() const override { return true; }
    pmt_t car() const { return d_car; }
    pmt_t cdr() const { return d_cdr; }

    void set_car(pmt_t car) { d_car = car; }
    void set_cdr(pmt_t cdr) { d_cdr = cdr; }
};

class pmt_dict : public pmt_pair
{
public:
    pmt_dict(const pmt_t& car, const pmt_t& cdr);
    //~pmt_dict(){};

    bool is_dict() const override { return true; }
};

class pmt_vector : public pmt_base
{
    std::vector<pmt_t> d_v;

public:
    pmt_vector(size_t len, pmt_t fill);
    //~pmt_vector();

    bool is_vector() const override { return true; }
    pmt_t ref(size_t k) const;
    void set(size_t k, pmt_t obj);
    void fill(pmt_t fill);
    size_t length() const { return d_v.size(); }

    pmt_t _ref(size_t k) const { return d_v[k]; }
};

class pmt_tuple : public pmt_base
{
    std::vector<pmt_t> d_v;

public:
    pmt_tuple(size_t len);
    //~pmt_tuple();

    bool is_tuple() const override { return true; }
    pmt_t ref(size_t k) const;
    size_t length() const { return d_v.size(); }

    pmt_t _ref(size_t k) const { return d_v[k]; }
    void _set(size_t k, pmt_t v) { d_v[k] = v; }
};

class pmt_any : public pmt_base
{
    std::any d_any;

public:
    pmt_any(const std::any& any);
    //~pmt_any();

    bool is_any() const override { return true; }
    const std::any& ref() const { return d_any; }
    void set(const std::any& any) { d_any = any; }
};


class pmt_uniform_vector : public pmt_base
{
public:
    bool is_uniform_vector() const override { return true; }
    virtual const void* uniform_elements(size_t& len) = 0;
    virtual void* uniform_writable_elements(size_t& len) = 0;
    virtual size_t length() const = 0;
    virtual size_t itemsize() const = 0;
    virtual const std::string string_ref(size_t k) const
    {
        return std::string("not implemented");
    }
};
} /* namespace pmt */

#endif /* INCLUDED_PMT_INT_H */
