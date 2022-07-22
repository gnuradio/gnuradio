/* -*- c++ -*- */
/*
 * Copyright 2013,2020 Free Software Foundation, Inc.
 * Copyright 2021 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#include <gnuradio/python_block.h>
#include <pybind11/complex.h>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace gr {

/**************************************************************************
 *   Python Block
 **************************************************************************/
python_block::sptr python_block::make(const py::object& p, const std::string& name)
{
    return std::make_shared<python_block>(p, name);
}

python_block::python_block(const py::handle& p, const std::string& name) : block(name)
{
    d_py_handle = p;
}

work_return_code_t python_block::work(work_io& wio)
{
    py::gil_scoped_acquire acquire;

    py::object ret = d_py_handle.attr("handle_work")(&wio);

    return ret.cast<work_return_code_t>();
}

bool python_block::start(void)
{
    py::gil_scoped_acquire acquire;

    py::object ret = d_py_handle.attr("start")();
    return ret.cast<bool>();
}

bool python_block::stop(void)
{
    py::gil_scoped_acquire acquire;

    py::object ret = d_py_handle.attr("stop")();
    return ret.cast<bool>();
}

/**************************************************************************
 *   Python Sync Block
 **************************************************************************/
python_sync_block::sptr python_sync_block::make(const py::object& p,
                                                const std::string& name)
{
    return std::make_shared<python_sync_block>(p, name);
}

python_sync_block::python_sync_block(const py::handle& p, const std::string& name)
    : sync_block(name)
{
    d_py_handle = p;
}


work_return_code_t python_sync_block::work(work_io& wio)
{
    py::gil_scoped_acquire acquire;

    auto ww = d_py_handle.attr("work");
    py::object ret = ww(&wio);

    return ret.cast<work_return_code_t>();
}

bool python_sync_block::start(void)
{
    py::gil_scoped_acquire acquire;

    py::object ret = d_py_handle.attr("start")();
    return ret.cast<bool>();
}

bool python_sync_block::stop(void)
{
    py::gil_scoped_acquire acquire;

    py::object ret = d_py_handle.attr("stop")();
    return ret.cast<bool>();
}


} /* namespace gr */
