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

#include <gnuradio/digital/additive_scrambler.h>
// pydoc.h is automatically generated in the build directory
#include <additive_scrambler_pydoc.h>

template <class T>
static void bind_additive_scrambler_template(py::module& m, const char* classname)
{
    using additive_scrambler = ::gr::digital::additive_scrambler<T>;

    py::class_<additive_scrambler,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<additive_scrambler>>(m, classname, D(additive_scrambler))
        .def(py::init(&additive_scrambler::make),
             py::arg("mask"),
             py::arg("seed"),
             py::arg("len"),
             py::arg("count") = 0,
             py::arg("bits_per_byte") = 1,
             py::arg("reset_tag_key") = "",
             D(additive_scrambler, make))
        .def("mask", &additive_scrambler::mask, D(additive_scrambler, mask))
        .def("seed", &additive_scrambler::seed, D(additive_scrambler, seed))
        .def("len", &additive_scrambler::len, D(additive_scrambler, len))
        .def("count", &additive_scrambler::count, D(additive_scrambler, count))
        .def("bits_per_byte",
             &additive_scrambler::bits_per_byte,
             D(additive_scrambler, bits_per_byte));
}

void bind_additive_scrambler(py::module& m)
{
    bind_additive_scrambler_template<gr_complex>(m, "additive_scrambler_cc");
    bind_additive_scrambler_template<float>(m, "additive_scrambler_ff");
    bind_additive_scrambler_template<int32_t>(m, "additive_scrambler_ii");
    bind_additive_scrambler_template<int16_t>(m, "additive_scrambler_ss");
    bind_additive_scrambler_template<uint8_t>(m, "additive_scrambler_bb");
}
