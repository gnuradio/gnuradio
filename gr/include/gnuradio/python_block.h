/* -*- c++ -*- */
/*
 * Copyright 2011-2013,2017,2020 Free Software Foundation, Inc.
 * Copyright 2021 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/api.h>
#include <gnuradio/block.h>
#include <gnuradio/sync_block.h>
#include <string>

#include <pybind11/pybind11.h> // must be first
#include <pybind11/stl.h>
namespace py = pybind11;

namespace gr {

/*!
 * The gateway block which performs all the magic.
 *
 * The gateway provides access to all the gr::block routines.
 */
enum py_block_t { PY_BLOCK_GENERAL = 0, PY_BLOCK_SYNC, PY_BLOCK_DECIM, PY_BLOCK_INTERP };


class GR_RUNTIME_API python_block : virtual public gr::block
{
private:
    py::handle d_py_handle;

public:
    using sptr = std::shared_ptr<python_block>;
    python_block(const py::handle& p, const std::string& name);
    static sptr make(const py::object& py_handle, const std::string& name);

    /*******************************************************************
     * Overloads for various scheduler-called functions
     ******************************************************************/
    work_return_code_t work(work_io& wio) override;

    bool start(void) override;
    bool stop(void) override;
};

class GR_RUNTIME_API python_sync_block : virtual public gr::sync_block
{
private:
    py::handle d_py_handle;

public:
    // gr::python_sync_block::sptr
    using sptr = std::shared_ptr<python_sync_block>;
    python_sync_block(const py::handle& p, const std::string& name);
    static sptr make(const py::object& py_handle, const std::string& name);

    /*******************************************************************
     * Overloads for various scheduler-called functions
     ******************************************************************/
    work_return_code_t work(work_io& wio) override;

    bool start(void) override;
    bool stop(void) override;
};

} /* namespace gr */
