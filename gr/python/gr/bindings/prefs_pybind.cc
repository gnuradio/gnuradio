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

#include <gnuradio/prefs.h>

void bind_prefs(py::module& m)
{

    using prefs = ::gr::prefs;


    py::class_<prefs>(m, "prefs")

        // .def(py::init(&prefs::get_instance))

        .def_static("get_string", &prefs::get_string)
        .def_static("get_bool", &prefs::get_bool)
        .def_static("get_long", &prefs::get_long)
        .def_static("get_double", &prefs::get_double);
}
