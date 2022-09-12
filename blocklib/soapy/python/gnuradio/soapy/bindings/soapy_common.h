/*
 * Copyright 2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <pybind11/complex.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/soapy/soapy_types.h>

py::object cast_string_to_arginfo_type(gr::soapy::argtype_t argtype,
                                       const std::string& str);

struct setting_info {
    std::string value;
    gr::soapy::argtype_t type;
};

setting_info cast_pyobject_to_arginfo_string(py::object obj);
