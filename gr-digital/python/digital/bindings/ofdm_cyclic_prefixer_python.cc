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
/* BINDTOOL_HEADER_FILE(ofdm_cyclic_prefixer.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(a09af8ad019f0f06d8d58d2bedbd7570)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/digital/ofdm_cyclic_prefixer.h>
// pydoc.h is automatically generated in the build directory
#include <ofdm_cyclic_prefixer_pydoc.h>

void bind_ofdm_cyclic_prefixer(py::module& m)
{
    using ofdm_cyclic_prefixer = gr::digital::ofdm_cyclic_prefixer;


    py::class_<ofdm_cyclic_prefixer,
               gr::tagged_stream_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<ofdm_cyclic_prefixer>>(m, "ofdm_cyclic_prefixer")

        .def(py::init((std::shared_ptr<ofdm_cyclic_prefixer>(*)(
                          size_t, size_t, int, const std::string&)) &
                      ofdm_cyclic_prefixer::make),
             py::arg("input_size"),
             py::arg("output_size"),
             py::arg("rolloff_len") = 0,
             py::arg("len_tag_key") = "",
             D(ofdm_cyclic_prefixer, make, 0))

        .def(py::init((std::shared_ptr<ofdm_cyclic_prefixer>(*)(
                          int, const std::vector<int>&, int, const std::string&)) &
                      ofdm_cyclic_prefixer::make),
             py::arg("input_size"),
             py::arg("output_size"),
             py::arg("rolloff_len") = 0,
             py::arg("len_tag_key") = "",
             D(ofdm_cyclic_prefixer, make, 1));
}
