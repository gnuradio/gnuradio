/* -*- c++ -*- */
/*
 * Copyright 2006,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_PY_FEVAL_H
#define INCLUDED_GR_PY_FEVAL_H

#include <gnuradio/feval.h>
#include <Python.h>
#include <pmt/pmt.h>

class ensure_py_gil_state
{
    PyGILState_STATE d_gstate;

public:
    ensure_py_gil_state() { d_gstate = PyGILState_Ensure(); }
    ~ensure_py_gil_state() { PyGILState_Release(d_gstate); }
};

namespace gr {

class GR_RUNTIME_API py_feval_dd : public feval_dd
{
public:
    double calleval(double x)
    {
        ensure_py_gil_state _lock;
        return eval(x);
    }
    virtual ~py_feval_dd(){};
};

class GR_RUNTIME_API py_feval_cc : public feval_cc
{
public:
    gr_complex calleval(gr_complex x)
    {
        ensure_py_gil_state _lock;
        return eval(x);
    }
    virtual ~py_feval_cc(){};
};

class GR_RUNTIME_API py_feval_ll : public feval_ll
{
public:
    long calleval(long x)
    {
        ensure_py_gil_state _lock;
        return eval(x);
    }
    virtual ~py_feval_ll(){};
};

class GR_RUNTIME_API py_feval : public feval
{
public:
    void calleval()
    {
        ensure_py_gil_state _lock;
        eval();
    }
    virtual ~py_feval(){};
};

class GR_RUNTIME_API py_feval_p : public feval_p
{
public:
    void calleval(pmt::pmt_t x)
    {
        ensure_py_gil_state _lock;
        eval(x);
    }
    virtual ~py_feval_p(){};
};

} /* namespace gr */

#endif /* INCLUDED_GR_PY_FEVAL_H */
