/*
 * Copyright 2020 Free Software Foundation, Inc.
 * Copyright 2025 Marcus Müller <mmueller@gnuradio.org>
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(interleaver.h)                                             */
/* BINDTOOL_HEADER_FILE_HASH(3563a5c4bff31076d1ad4d851d1e6712)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/trellis/interleaver.h>
// pydoc.h is automatically generated in the build directory
#include <interleaver_pydoc.h>

#include <gnuradio/version.h>

void bind_interleaver(py::module& m)
{

    using interleaver = ::gr::trellis::interleaver;
#if GR_VERSION < GR_MAKE_VERSION(3, 11, 0, 0)
    const char* interleaver_arg = "INTER";
    const char* length_arg = "K";
    const char* copy_arg = "INTERLEAVER";
#else
    const char* interleaver_arg = "inter";
    const char* length_arg = "k";
    const char* copy_arg = "interleaver";
#endif
    auto interleaver_bind =
        py::class_<interleaver, std::shared_ptr<interleaver>>(
            m, "interleaver", D(interleaver))
            .def(py::init<>(), D(interleaver, interleaver, 0))
            .def(py::init<gr::trellis::interleaver const&>(),
                 py::arg(copy_arg),
                 D(interleaver, interleaver, 1))
            .def(py::init<std::vector<int, std::allocator<int>> const&>(),
                 py::arg(interleaver_arg),
                 D(interleaver, interleaver, 2))
            .def(py::init<char const*>(), py::arg("name"), D(interleaver, interleaver, 3))
            .def(py::init<unsigned int, int>(),
                 py::arg(length_arg),
                 py::arg("seed"),
                 D(interleaver, interleaver, 4))
            .def("k", &interleaver::k, D(interleaver, k))
            .def("interleaver_indices",
                 &interleaver::interleaver_indices,
                 D(interleaver, interleaver_indices))
            .def("deinterleaver_indices",
                 &interleaver::deinterleaver_indices,
                 D(interleaver, interleaver_indices))
            .def("write_interleaver_txt",
                 &interleaver::write_interleaver_txt,
                 py::arg("filename"),
                 D(interleaver, write_interleaver_txt));

#if (GR_VERSION < GR_MAKE_VERSION(3, 11, 0, 0))
    // in GR 3.11, rename K->k, INTER->interleaver_taps
    // and DEINTER->deinterleaver_taps
    interleaver_bind.def("K", &interleaver::k, D(interleaver, k))
        .def("INTER",
             &interleaver::interleaver_indices,
             D(interleaver, interleaver_indices))
        .def("DEINTER",
             &interleaver::deinterleaver_indices,
             D(interleaver, deinterleaver_indices));
    // Deprecated constructor that copies a vector, but also gets a int as length of the
    // vector, which might disagree with the reality of the vector.
    interleaver_bind.def(
        py::init<unsigned int, std::vector<int, std::allocator<int>> const&>(),
        py::arg(length_arg),
        py::arg(interleaver_arg),
        D(interleaver, interleaver, 2));
#endif
}
