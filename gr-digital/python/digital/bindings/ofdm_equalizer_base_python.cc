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
/* BINDTOOL_HEADER_FILE(ofdm_equalizer_base.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(65983532ef2b3a47c2c295e23fdbed37)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/digital/ofdm_equalizer_base.h>
// pydoc.h is automatically generated in the build directory
#include <ofdm_equalizer_base_pydoc.h>

void bind_ofdm_equalizer_base(py::module& m)
{

    using ofdm_equalizer_base = ::gr::digital::ofdm_equalizer_base;
    using ofdm_equalizer_1d_pilots = ::gr::digital::ofdm_equalizer_1d_pilots;


    py::class_<ofdm_equalizer_base, std::shared_ptr<ofdm_equalizer_base>>(
        m, "ofdm_equalizer_base", D(ofdm_equalizer_base))

        // .def(py::init<int>(),           py::arg("fft_len"),
        //    D(ofdm_equalizer_base,ofdm_equalizer_base,0)
        // )
        // .def(py::init<gr::digital::ofdm_equalizer_base const &>(), py::arg("arg0"),
        //    D(ofdm_equalizer_base,ofdm_equalizer_base,1)
        // )


        .def("reset", &ofdm_equalizer_base::reset, D(ofdm_equalizer_base, reset))


        .def("equalize",
             &ofdm_equalizer_base::equalize,
             py::arg("frame"),
             py::arg("n_sym"),
             py::arg("initial_taps") = std::vector<gr_complex>(),
             py::arg("tags") = std::vector<gr::tag_t>(),
             D(ofdm_equalizer_base, equalize))


        .def("get_channel_state",
             &ofdm_equalizer_base::get_channel_state,
             py::arg("taps"),
             D(ofdm_equalizer_base, get_channel_state))


        .def("fft_len", &ofdm_equalizer_base::fft_len, D(ofdm_equalizer_base, fft_len))


        .def("base", &ofdm_equalizer_base::base, D(ofdm_equalizer_base, base))

        ;


    py::class_<ofdm_equalizer_1d_pilots,
               gr::digital::ofdm_equalizer_base,
               std::shared_ptr<ofdm_equalizer_1d_pilots>>(
        m, "ofdm_equalizer_1d_pilots", D(ofdm_equalizer_1d_pilots))

        // .def(py::init<int,std::vector<std::vector<int, std::allocator<int> >,
        // std::allocator<std::vector<int, std::allocator<int> > > > const
        // &,std::vector<std::vector<int, std::allocator<int> >,
        // std::allocator<std::vector<int, std::allocator<int> > > > const
        // &,std::vector<std::vector<std::complex<float>,
        // std::allocator<std::complex<float> > >,
        // std::allocator<std::vector<std::complex<float>,
        // std::allocator<std::complex<float> > > > > const &,int,bool>(),
        // py::arg("fft_len"),
        //    py::arg("occupied_carriers"),
        //    py::arg("pilot_carriers"),
        //    py::arg("pilot_symbols"),
        //    py::arg("symbols_skipped"),
        //    py::arg("input_is_shifted"),
        //    D(ofdm_equalizer_1d_pilots,ofdm_equalizer_1d_pilots,0)
        // )
        // .def(py::init<gr::digital::ofdm_equalizer_1d_pilots const &>(),
        // py::arg("arg0"),
        //    D(ofdm_equalizer_1d_pilots,ofdm_equalizer_1d_pilots,1)
        // )


        .def(
            "reset", &ofdm_equalizer_1d_pilots::reset, D(ofdm_equalizer_1d_pilots, reset))


        .def("get_channel_state",
             &ofdm_equalizer_1d_pilots::get_channel_state,
             py::arg("taps"),
             D(ofdm_equalizer_1d_pilots, get_channel_state))

        ;
}
