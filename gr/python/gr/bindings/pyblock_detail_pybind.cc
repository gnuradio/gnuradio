/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/pyblock_detail.h>

void bind_pyblock_detail(py::module& m)
{
    using pyblock_detail = ::gr::pyblock_detail;

    py::class_<pyblock_detail, std::shared_ptr<pyblock_detail>>(m, "pyblock_detail")
        .def(py::init<py::handle>());
}
