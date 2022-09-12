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

#include <gnuradio/kernel/math/random.h>

// Put this somewhere else
#define __EXPAND(x) x
#define __COUNT(_1, _2, _3, _4, _5, _6, _7, COUNT, ...) COUNT
#define __VA_SIZE(...) __EXPAND(__COUNT(__VA_ARGS__, 7, 6, 5, 4, 3, 2, 1))
#define __CAT1(a, b) a##b
#define __CAT2(a, b) __CAT1(a, b)
#define __DOC1(n1) ""
#define __DOC2(n1, n2) ""
#define __DOC3(n1, n2, n3) ""
#define __DOC4(n1, n2, n3, n4) ""
#define __DOC5(n1, n2, n3, n4, n5) ""
#define __DOC6(n1, n2, n3, n4, n5, n6) ""
#define __DOC7(n1, n2, n3, n4, n5, n6, n7) ""
#define DOC(...) __EXPAND(__EXPAND(__CAT2(__DOC, __VA_SIZE(__VA_ARGS__)))(__VA_ARGS__))
#define D(...) DOC(gr, __VA_ARGS__)


void bind_random(py::module& m)
{
    using xoroshiro128p_prng = ::gr::kernel::math::xoroshiro128p_prng;
    using random = ::gr::kernel::math::random;


    py::class_<xoroshiro128p_prng, std::shared_ptr<xoroshiro128p_prng>>(
        m, "xoroshiro128p_prng", D(xoroshiro128p_prng))

        .def(py::init<uint64_t>(),
             py::arg("init"),
             D(xoroshiro128p_prng, xoroshiro128p_prng, 0))
        .def(py::init<xoroshiro128p_prng const&>(),
             py::arg("arg0"),
             D(xoroshiro128p_prng, xoroshiro128p_prng, 1))


        .def_static("min", &xoroshiro128p_prng::min, D(xoroshiro128p_prng, min))


        .def_static("max", &xoroshiro128p_prng::max, D(xoroshiro128p_prng, max))


        .def("seed",
             &xoroshiro128p_prng::seed,
             py::arg("seed"),
             D(xoroshiro128p_prng, seed))

        .def("__call__", &xoroshiro128p_prng::operator());


    py::class_<random, std::shared_ptr<random>>(m, "random", D(random))

        .def(py::init<uint64_t, int64_t, int64_t>(),
             py::arg("seed") = 0,
             py::arg("min_integer") = 0,
             py::arg("max_integer") = 2,
             D(random, random, 0))
        .def(py::init<random const&>(), py::arg("arg0"), D(random, random, 1))


        .def("reseed", &random::reseed, py::arg("seed"), D(random, reseed))


        .def("set_integer_limits",
             &random::set_integer_limits,
             py::arg("minimum"),
             py::arg("maximum"),
             D(random, set_integer_limits))


        .def("ran_int", &random::ran_int, D(random, ran_int))


        .def("ran1", &random::ran1, D(random, ran1))


        .def("gasdev", &random::gasdev, D(random, gasdev))


        .def("laplacian", &random::laplacian, D(random, laplacian))


        .def("rayleigh", &random::rayleigh, D(random, rayleigh))


        .def("impulse", &random::impulse, py::arg("factor"), D(random, impulse))


        .def("rayleigh_complex", &random::rayleigh_complex, D(random, rayleigh_complex))

        ;
}
