/*
 * Copyright 2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "soapy_common.h"

#include <pybind11/complex.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/soapy/soapy_types.h>

void bind_soapy_types(py::module& m)
{
    py::class_<gr::soapy::range_t>(m, "range_t")
        // Constructors
        .def(py::init<>())
        .def(py::init<double, double>())
        .def(py::init<double, double, double>())

        // Methods
        .def("minimum", &gr::soapy::range_t::minimum)
        .def("maximum", &gr::soapy::range_t::maximum)
        .def("step", &gr::soapy::range_t::step)

        .def("__str__", [](const gr::soapy::range_t& range) -> std::string {
            std::string ret = "(minimum: ";
            ret += std::to_string(range.minimum());
            ret += ", maximum: ";
            ret += std::to_string(range.maximum());
            ret += ", step: ";
            ret += std::to_string(range.step());
            ret += ")";

            return ret;
        });

    py::enum_<gr::soapy::argtype_t>(m, "argtype_t")
        .value("BOOL", gr::soapy::arginfo_t::BOOL)
        .value("INT", gr::soapy::arginfo_t::INT)
        .value("FLOAT", gr::soapy::arginfo_t::FLOAT)
        .value("STRING", gr::soapy::arginfo_t::STRING)
        .export_values();

    py::class_<gr::soapy::arginfo_t>(m, "arginfo_t")
        // Constructors
        .def(py::init<>())

        // Properties
        .def_readwrite("key", &gr::soapy::arginfo_t::key)
        .def_property(
            "value",
            [](const gr::soapy::arginfo_t& arginfo) -> py::object {
                return cast_string_to_arginfo_type(arginfo.type, arginfo.value);
            },
            // So we can implicitly convert to Soapy's convention
            [](gr::soapy::arginfo_t& arginfo, py::object obj) -> void {
                const auto info = cast_pyobject_to_arginfo_string(obj);

                arginfo.value = info.value;
                arginfo.type = info.type;
            })

        .def_readwrite("name", &gr::soapy::arginfo_t::name)
        .def_readwrite("description", &gr::soapy::arginfo_t::description)
        .def_readwrite("units", &gr::soapy::arginfo_t::units)
        .def_readwrite("type", &gr::soapy::arginfo_t::type)
        .def_readwrite("range", &gr::soapy::arginfo_t::range)
        .def_readwrite("options", &gr::soapy::arginfo_t::options)
        .def_readwrite("option_names", &gr::soapy::arginfo_t::optionNames)

        .def("__str__", [](const gr::soapy::arginfo_t& arginfo) -> std::string {
            return (arginfo.key + "=" + arginfo.value);
        });
}
