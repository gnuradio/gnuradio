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
/* BINDTOOL_HEADER_FILE(wavfile.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(dab703290a81f206f7c5c32779078630)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/blocks/wavfile.h>
// pydoc.h is automatically generated in the build directory
#include <wavfile_pydoc.h>

void bind_wavfile(py::module& m)
{
    using wav_header_info = ::gr::blocks::wav_header_info;

    py::class_<wav_header_info, std::shared_ptr<wav_header_info>>(m, "wav_header_info")
        .def_readwrite("sample_rate", &wav_header_info::sample_rate)
        .def_readwrite("nchans", &wav_header_info::nchans)
        .def_readwrite("bytes_per_sample", &wav_header_info::bytes_per_sample)
        .def_readwrite("samples_per_chan", &wav_header_info::samples_per_chan)
        .def_readwrite("format", &wav_header_info::format)
        .def_readwrite("subformat", &wav_header_info::subformat);

    py::enum_<::gr::blocks::wavfile_format_t>(m, "wavfile_format_t")
        .value("FORMAT_WAV", ::gr::blocks::FORMAT_WAV)   // 65536
        .value("FORMAT_FLAC", ::gr::blocks::FORMAT_FLAC) // 1507328
        .value("FORMAT_OGG", ::gr::blocks::FORMAT_OGG)   // 2097152
        .value("FORMAT_RF64", ::gr::blocks::FORMAT_RF64) // 2228224
        .export_values();

    py::enum_<::gr::blocks::wavfile_subformat_t>(m, "wavfile_subformat_t")
        .value("FORMAT_PCM_S8", ::gr::blocks::FORMAT_PCM_S8) // 1
        .value("FORMAT_PCM_16", ::gr::blocks::FORMAT_PCM_16) // 2
        .value("FORMAT_PCM_24", ::gr::blocks::FORMAT_PCM_24) // 3
        .value("FORMAT_PCM_32", ::gr::blocks::FORMAT_PCM_32) // 4
        .value("FORMAT_PCM_U8", ::gr::blocks::FORMAT_PCM_U8) // 5
        .value("FORMAT_FLOAT", ::gr::blocks::FORMAT_FLOAT)   // 6
        .value("FORMAT_DOUBLE", ::gr::blocks::FORMAT_DOUBLE) // 7
        .value("FORMAT_VORBIS", ::gr::blocks::FORMAT_VORBIS) // 96
        .value("FORMAT_OPUS", ::gr::blocks::FORMAT_OPUS)     // 100
        .export_values();

    py::implicitly_convertible<int, ::gr::blocks::wavfile_format_t>();
    py::implicitly_convertible<int, ::gr::blocks::wavfile_subformat_t>();
}
