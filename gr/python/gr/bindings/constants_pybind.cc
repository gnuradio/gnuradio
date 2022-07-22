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

#include <gnuradio/constants.h>
// pydoc.h is automatically generated in the build directory
// #include <edge_pydoc.h>

void bind_constants(py::module& m)
{
    m.def("prefix", &::gr::prefix);
    m.def("version", &::gr::version);
    m.def("major_version", &::gr::major_version);
    m.def("api_version", &::gr::api_version);
    m.def("minor_version", &::gr::minor_version);
}
