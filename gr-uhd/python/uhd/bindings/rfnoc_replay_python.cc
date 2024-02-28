/*
 * Copyright 2024 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually
 * edited  */
/* The following lines can be configured to regenerate this file during cmake */
/* If manual edits are made, the following tags should be modified accordingly.
 */
/* BINDTOOL_GEN_AUTOMATIC(0) */
/* BINDTOOL_USE_PYGCCXML(0) */
/* BINDTOOL_HEADER_FILE(rfnoc_replay.h) */
/* BINDTOOL_HEADER_FILE_HASH(5a54ed341c72e79cb1785ea9e8fe0001) */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/uhd/rfnoc_replay.h>
// pydoc.h is automatically generated in the build directory
#include <rfnoc_replay_pydoc.h>

void bind_rfnoc_replay(py::module& m)
{

    using rfnoc_replay = ::gr::uhd::rfnoc_replay;

    py::class_<rfnoc_replay,
               gr::uhd::rfnoc_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<rfnoc_replay>>(m, "rfnoc_replay", D(rfnoc_replay))

        .def(py::init(&rfnoc_replay::make),
             py::arg("graph"),
             py::arg("block_args"),
             py::arg("device_select"),
             py::arg("instance"),
             D(rfnoc_replay, make))

        .def("record",
             &rfnoc_replay::record,
             py::arg("offset"),
             py::arg("size"),
             py::arg("port") = 0,
             D(rfnoc_replay, record))

        .def("record_restart",
             &rfnoc_replay::record_restart,
             py::arg("port") = 0,
             D(rfnoc_replay, record_restart))

        .def("play",
             &rfnoc_replay::play,
             py::arg("offset"),
             py::arg("size"),
             py::arg("port") = 0,
             py::arg("time_spec") = ::uhd::time_spec_t(0.),
             py::arg("repeat") = false,
             D(rfnoc_replay, play))

        .def("stop_playback",
             &rfnoc_replay::stop_playback,
             py::arg("port") = 0,
             D(rfnoc_replay, stop_playback))

        .def("set_record_type",
             &rfnoc_replay::set_record_type,
             py::arg("type"),
             py::arg("port") = 0,
             D(rfnoc_replay, set_record_type))

        .def("set_play_type",
             &rfnoc_replay::set_play_type,
             py::arg("type"),
             py::arg("port") = 0,
             D(rfnoc_replay, set_play_type))

        .def("issue_stream_cmd",
             &rfnoc_replay::issue_stream_cmd,
             py::arg("cmd"),
             py::arg("port") = 0,
             D(rfnoc_replay, issue_stream_cmd))

        ;

    m.def("replay_cmd_key", &::gr::uhd::replay_cmd_key, D(replay_cmd_key));

    m.def("replay_cmd_port_key", &::gr::uhd::replay_cmd_port_key, D(replay_cmd_port_key));

    m.def("replay_cmd_offset_key",
          &::gr::uhd::replay_cmd_offset_key,
          D(replay_cmd_offset_key));

    m.def("replay_cmd_size_key", &::gr::uhd::replay_cmd_size_key, D(replay_cmd_size_key));

    m.def("replay_cmd_time_key", &::gr::uhd::replay_cmd_time_key, D(replay_cmd_time_key));

    m.def("replay_cmd_repeat_key",
          &::gr::uhd::replay_cmd_repeat_key,
          D(replay_cmd_repeat_key));

    m.def("replay_debug_port_key",
          &::gr::uhd::replay_debug_port_key,
          D(replay_debug_port_key));

    m.def("replay_mem_fullness_key",
          &::gr::uhd::replay_mem_fullness_key,
          D(replay_mem_fullness_key));

    m.def("replay_mem_size_key", &::gr::uhd::replay_mem_size_key, D(replay_mem_size_key));

    m.def("replay_word_size_key",
          &::gr::uhd::replay_word_size_key,
          D(replay_word_size_key));
}
