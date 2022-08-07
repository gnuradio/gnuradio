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
/* BINDTOOL_HEADER_FILE(firdes.h)                                                  */
/* BINDTOOL_HEADER_FILE_HASH(be6fcf3be3d516d7f3e0ebac0c929c82)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/filter/firdes.h>
// pydoc.h is automatically generated in the build directory
#include <firdes_pydoc.h>

void bind_firdes(py::module& m)
{
    using firdes = gr::filter::firdes;

    py::class_<firdes, std::shared_ptr<firdes>> firdes_class(m, "firdes", D(firdes));

    firdes_class
        .def_static("window",
                    &firdes::window,
                    py::arg("type"),
                    py::arg("ntaps"),
                    py::arg("param"),
                    D(firdes, window))


        .def_static("low_pass",
                    &firdes::low_pass,
                    py::arg("gain"),
                    py::arg("sampling_freq"),
                    py::arg("cutoff_freq"),
                    py::arg("transition_width"),
                    py::arg("window") = ::gr::fft::window::win_type::WIN_HAMMING,
                    py::arg("param") = 6.7599999999999998,
                    D(firdes, low_pass))


        .def_static("low_pass_2",
                    &firdes::low_pass_2,
                    py::arg("gain"),
                    py::arg("sampling_freq"),
                    py::arg("cutoff_freq"),
                    py::arg("transition_width"),
                    py::arg("attenuation_dB"),
                    py::arg("window") = ::gr::fft::window::win_type::WIN_HAMMING,
                    py::arg("param") = 6.7599999999999998,
                    D(firdes, low_pass_2))


        .def_static("high_pass",
                    &firdes::high_pass,
                    py::arg("gain"),
                    py::arg("sampling_freq"),
                    py::arg("cutoff_freq"),
                    py::arg("transition_width"),
                    py::arg("window") = ::gr::fft::window::win_type::WIN_HAMMING,
                    py::arg("param") = 6.7599999999999998,
                    D(firdes, high_pass))


        .def_static("high_pass_2",
                    &firdes::high_pass_2,
                    py::arg("gain"),
                    py::arg("sampling_freq"),
                    py::arg("cutoff_freq"),
                    py::arg("transition_width"),
                    py::arg("attenuation_dB"),
                    py::arg("window") = ::gr::fft::window::win_type::WIN_HAMMING,
                    py::arg("param") = 6.7599999999999998,
                    D(firdes, high_pass_2))


        .def_static("band_pass",
                    &firdes::band_pass,
                    py::arg("gain"),
                    py::arg("sampling_freq"),
                    py::arg("low_cutoff_freq"),
                    py::arg("high_cutoff_freq"),
                    py::arg("transition_width"),
                    py::arg("window") = ::gr::fft::window::win_type::WIN_HAMMING,
                    py::arg("param") = 6.7599999999999998,
                    D(firdes, band_pass))


        .def_static("band_pass_2",
                    &firdes::band_pass_2,
                    py::arg("gain"),
                    py::arg("sampling_freq"),
                    py::arg("low_cutoff_freq"),
                    py::arg("high_cutoff_freq"),
                    py::arg("transition_width"),
                    py::arg("attenuation_dB"),
                    py::arg("window") = ::gr::fft::window::win_type::WIN_HAMMING,
                    py::arg("param") = 6.7599999999999998,
                    D(firdes, band_pass_2))


        .def_static("complex_band_pass",
                    &firdes::complex_band_pass,
                    py::arg("gain"),
                    py::arg("sampling_freq"),
                    py::arg("low_cutoff_freq"),
                    py::arg("high_cutoff_freq"),
                    py::arg("transition_width"),
                    py::arg("window") = ::gr::fft::window::win_type::WIN_HAMMING,
                    py::arg("param") = 6.7599999999999998,
                    D(firdes, complex_band_pass))


        .def_static("complex_band_pass_2",
                    &firdes::complex_band_pass_2,
                    py::arg("gain"),
                    py::arg("sampling_freq"),
                    py::arg("low_cutoff_freq"),
                    py::arg("high_cutoff_freq"),
                    py::arg("transition_width"),
                    py::arg("attenuation_dB"),
                    py::arg("window") = ::gr::fft::window::win_type::WIN_HAMMING,
                    py::arg("param") = 6.7599999999999998,
                    D(firdes, complex_band_pass_2))


        .def_static("band_reject",
                    &firdes::band_reject,
                    py::arg("gain"),
                    py::arg("sampling_freq"),
                    py::arg("low_cutoff_freq"),
                    py::arg("high_cutoff_freq"),
                    py::arg("transition_width"),
                    py::arg("window") = ::gr::fft::window::win_type::WIN_HAMMING,
                    py::arg("param") = 6.7599999999999998,
                    D(firdes, band_reject))


        .def_static("band_reject_2",
                    &firdes::band_reject_2,
                    py::arg("gain"),
                    py::arg("sampling_freq"),
                    py::arg("low_cutoff_freq"),
                    py::arg("high_cutoff_freq"),
                    py::arg("transition_width"),
                    py::arg("attenuation_dB"),
                    py::arg("window") = ::gr::fft::window::win_type::WIN_HAMMING,
                    py::arg("param") = 6.7599999999999998,
                    D(firdes, band_reject_2))


        .def_static("complex_band_reject",
                    &firdes::complex_band_reject,
                    py::arg("gain"),
                    py::arg("sampling_freq"),
                    py::arg("low_cutoff_freq"),
                    py::arg("high_cutoff_freq"),
                    py::arg("transition_width"),
                    py::arg("window") = ::gr::fft::window::win_type::WIN_HAMMING,
                    py::arg("param") = 6.7599999999999998,
                    D(firdes, complex_band_reject))


        .def_static("complex_band_reject_2",
                    &firdes::complex_band_reject_2,
                    py::arg("gain"),
                    py::arg("sampling_freq"),
                    py::arg("low_cutoff_freq"),
                    py::arg("high_cutoff_freq"),
                    py::arg("transition_width"),
                    py::arg("attenuation_dB"),
                    py::arg("window") = ::gr::fft::window::win_type::WIN_HAMMING,
                    py::arg("param") = 6.7599999999999998,
                    D(firdes, complex_band_reject_2))


        .def_static("hilbert",
                    &firdes::hilbert,
                    py::arg("ntaps") = 19,
                    py::arg("windowtype") = ::gr::fft::window::win_type::WIN_RECTANGULAR,
                    py::arg("param") = 6.7599999999999998,
                    D(firdes, hilbert))


        .def_static("root_raised_cosine",
                    &firdes::root_raised_cosine,
                    py::arg("gain"),
                    py::arg("sampling_freq"),
                    py::arg("symbol_rate"),
                    py::arg("alpha"),
                    py::arg("ntaps"),
                    D(firdes, root_raised_cosine))


        .def_static("gaussian",
                    &firdes::gaussian,
                    py::arg("gain"),
                    py::arg("spb"),
                    py::arg("bt"),
                    py::arg("ntaps"),
                    D(firdes, gaussian))

        ;
}
