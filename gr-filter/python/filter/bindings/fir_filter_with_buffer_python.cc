/*
 * Copyright 2020 Free Software Foundation, Inc.
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
/* BINDTOOL_HEADER_FILE(fir_filter_with_buffer.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(bb3571db4265c8c55abf11850978e0de)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/filter/fir_filter_with_buffer.h>
// pydoc.h is automatically generated in the build directory
#include <fir_filter_with_buffer_pydoc.h>

void bind_fir_filter_with_buffer(py::module& m)
{
    py::module m_kernel = m.def_submodule("kernel");

    using fir_filter_with_buffer_fff = gr::filter::kernel::fir_filter_with_buffer_fff;
    using fir_filter_with_buffer_ccc = gr::filter::kernel::fir_filter_with_buffer_ccc;
    using fir_filter_with_buffer_ccf = gr::filter::kernel::fir_filter_with_buffer_ccf;


    py::class_<fir_filter_with_buffer_fff, std::shared_ptr<fir_filter_with_buffer_fff>>(
        m_kernel, "fir_filter_with_buffer_fff", D(kernel, fir_filter_with_buffer_fff))

        .def(py::init<std::vector<float, std::allocator<float>> const&>(),
             py::arg("taps"),
             D(kernel, fir_filter_with_buffer_fff, fir_filter_with_buffer_fff, 0))
        .def(py::init<gr::filter::kernel::fir_filter_with_buffer_fff const&>(),
             py::arg("arg0"),
             D(kernel, fir_filter_with_buffer_fff, fir_filter_with_buffer_fff, 1))


        .def("filter",
             (float (fir_filter_with_buffer_fff::*)(float)) &
                 fir_filter_with_buffer_fff::filter,
             py::arg("input"),
             D(kernel, fir_filter_with_buffer_fff, filter, 0))


        .def("filter",
             (float (fir_filter_with_buffer_fff::*)(float const*, long unsigned int)) &
                 fir_filter_with_buffer_fff::filter,
             py::arg("input"),
             py::arg("dec"),
             D(kernel, fir_filter_with_buffer_fff, filter, 1))
        .def("filterN",
             &fir_filter_with_buffer_fff::filterN,
             py::arg("output"),
             py::arg("input"),
             py::arg("n"),
             D(kernel, fir_filter_with_buffer_fff, filterN))
        .def("filterNdec",
             &fir_filter_with_buffer_fff::filterNdec,
             py::arg("output"),
             py::arg("input"),
             py::arg("n"),
             py::arg("decimate"),
             D(kernel, fir_filter_with_buffer_fff, filterNdec))


        .def("ntaps",
             &fir_filter_with_buffer_fff::ntaps,
             D(kernel, fir_filter_with_buffer_fff, ntaps))


        .def("set_taps",
             &fir_filter_with_buffer_fff::set_taps,
             py::arg("taps"),
             D(kernel, fir_filter_with_buffer_fff, set_taps))


        .def("taps",
             &fir_filter_with_buffer_fff::taps,
             D(kernel, fir_filter_with_buffer_fff, taps))

        ;


    py::class_<fir_filter_with_buffer_ccc, std::shared_ptr<fir_filter_with_buffer_ccc>>(
        m_kernel, "fir_filter_with_buffer_ccc", D(kernel, fir_filter_with_buffer_ccc))

        .def(py::init<std::vector<std::complex<float>,
                                  std::allocator<std::complex<float>>> const&>(),
             py::arg("taps"),
             D(kernel, fir_filter_with_buffer_ccc, fir_filter_with_buffer_ccc, 0))
        .def(py::init<gr::filter::kernel::fir_filter_with_buffer_ccc const&>(),
             py::arg("arg0"),
             D(kernel, fir_filter_with_buffer_ccc, fir_filter_with_buffer_ccc, 1))


        .def("filter",
             (gr_complex(fir_filter_with_buffer_ccc::*)(gr_complex)) &
                 fir_filter_with_buffer_ccc::filter,
             py::arg("input"),
             D(kernel, fir_filter_with_buffer_ccc, filter, 0))


        .def("filter",
             (gr_complex(fir_filter_with_buffer_ccc::*)(gr_complex const*,
                                                        long unsigned int)) &
                 fir_filter_with_buffer_ccc::filter,
             py::arg("input"),
             py::arg("dec"),
             D(kernel, fir_filter_with_buffer_ccc, filter, 1))
        .def("filterN",
             &fir_filter_with_buffer_ccc::filterN,
             py::arg("output"),
             py::arg("input"),
             py::arg("n"),
             D(kernel, fir_filter_with_buffer_ccc, filterN))
        .def("filterNdec",
             &fir_filter_with_buffer_ccc::filterNdec,
             py::arg("output"),
             py::arg("input"),
             py::arg("n"),
             py::arg("decimate"),
             D(kernel, fir_filter_with_buffer_ccc, filterNdec))


        .def("ntaps",
             &fir_filter_with_buffer_ccc::ntaps,
             D(kernel, fir_filter_with_buffer_ccc, ntaps))


        .def("set_taps",
             &fir_filter_with_buffer_ccc::set_taps,
             py::arg("taps"),
             D(kernel, fir_filter_with_buffer_ccc, set_taps))


        .def("taps",
             &fir_filter_with_buffer_ccc::taps,
             D(kernel, fir_filter_with_buffer_ccc, taps))

        ;


    py::class_<fir_filter_with_buffer_ccf, std::shared_ptr<fir_filter_with_buffer_ccf>>(
        m_kernel, "fir_filter_with_buffer_ccf", D(kernel, fir_filter_with_buffer_ccf))

        .def(py::init<std::vector<float, std::allocator<float>> const&>(),
             py::arg("taps"),
             D(kernel, fir_filter_with_buffer_ccf, fir_filter_with_buffer_ccf, 0))
        .def(py::init<gr::filter::kernel::fir_filter_with_buffer_ccf const&>(),
             py::arg("arg0"),
             D(kernel, fir_filter_with_buffer_ccf, fir_filter_with_buffer_ccf, 1))


        .def("filter",
             (gr_complex(fir_filter_with_buffer_ccf::*)(gr_complex)) &
                 fir_filter_with_buffer_ccf::filter,
             py::arg("input"),
             D(kernel, fir_filter_with_buffer_ccf, filter, 0))


        .def("filter",
             (gr_complex(fir_filter_with_buffer_ccf::*)(gr_complex const*,
                                                        long unsigned int)) &
                 fir_filter_with_buffer_ccf::filter,
             py::arg("input"),
             py::arg("dec"),
             D(kernel, fir_filter_with_buffer_ccf, filter, 1))
        .def("filterN",
             &fir_filter_with_buffer_ccf::filterN,
             py::arg("output"),
             py::arg("input"),
             py::arg("n"),
             D(kernel, fir_filter_with_buffer_ccf, filterN))
        .def("filterNdec",
             &fir_filter_with_buffer_ccf::filterNdec,
             py::arg("output"),
             py::arg("input"),
             py::arg("n"),
             py::arg("decimate"),
             D(kernel, fir_filter_with_buffer_ccf, filterNdec))


        .def("ntaps",
             &fir_filter_with_buffer_ccf::ntaps,
             D(kernel, fir_filter_with_buffer_ccf, ntaps))


        .def("set_taps",
             &fir_filter_with_buffer_ccf::set_taps,
             py::arg("taps"),
             D(kernel, fir_filter_with_buffer_ccf, set_taps))


        .def("taps",
             &fir_filter_with_buffer_ccf::taps,
             D(kernel, fir_filter_with_buffer_ccf, taps))

        ;
}
