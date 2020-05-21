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
/* BINDTOOL_HEADER_FILE(hier_block2.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(367d4df8d747ae4040c2e5b46cbe1b4f)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/hier_block2.h>
// pydoc.h is automatically generated in the build directory
#include <hier_block2_pydoc.h>

void bind_hier_block2(py::module& m)
{

    using hier_block2 = ::gr::hier_block2;

    py::class_<hier_block2,
               gr::basic_block,
               gr::msg_accepter,
               std::shared_ptr<hier_block2>>(m, "hier_block2_pb", D(hier_block2))

        .def(py::init(&gr::make_hier_block2),
             py::arg("name"),
             py::arg("input_signature"),
             py::arg("output_signature"))

        .def("self", &hier_block2::self)
        .def("primitive_connect",
             (void (hier_block2::*)(gr::basic_block_sptr)) & hier_block2::connect,
             py::arg("block"))
        .def("primitive_connect",
             (void (hier_block2::*)(
                 gr::basic_block_sptr, int, gr::basic_block_sptr, int)) &
                 hier_block2::connect,
             py::arg("src"),
             py::arg("src_port"),
             py::arg("dst"),
             py::arg("dst_port"),
             D(hier_block2, connect, 1))
        .def("primitive_msg_connect",
             (void (hier_block2::*)(
                 gr::basic_block_sptr, pmt::pmt_t, gr::basic_block_sptr, pmt::pmt_t)) &
                 hier_block2::msg_connect,
             py::arg("src"),
             py::arg("srcport"),
             py::arg("dst"),
             py::arg("dstport"),
             D(hier_block2, msg_connect, 0))
        .def("primitive_msg_connect",
             (void (hier_block2::*)(
                 gr::basic_block_sptr, std::string, gr::basic_block_sptr, std::string)) &
                 hier_block2::msg_connect,
             py::arg("src"),
             py::arg("srcport"),
             py::arg("dst"),
             py::arg("dstport"),
             D(hier_block2, msg_connect, 1))
        .def("primitive_msg_disconnect",
             (void (hier_block2::*)(
                 gr::basic_block_sptr, pmt::pmt_t, gr::basic_block_sptr, pmt::pmt_t)) &
                 hier_block2::msg_disconnect,
             py::arg("src"),
             py::arg("srcport"),
             py::arg("dst"),
             py::arg("dstport"),
             D(hier_block2, msg_disconnect, 0))
        .def("primitive_msg_disconnect",
             (void (hier_block2::*)(
                 gr::basic_block_sptr, std::string, gr::basic_block_sptr, std::string)) &
                 hier_block2::msg_disconnect,
             py::arg("src"),
             py::arg("srcport"),
             py::arg("dst"),
             py::arg("dstport"),
             D(hier_block2, msg_disconnect, 1))
        .def("primitive_disconnect",
             (void (hier_block2::*)(gr::basic_block_sptr)) & hier_block2::disconnect,
             py::arg("block"),
             D(hier_block2, disconnect, 0))
        .def("primitive_disconnect",
             (void (hier_block2::*)(
                 gr::basic_block_sptr, int, gr::basic_block_sptr, int)) &
                 hier_block2::disconnect,
             py::arg("src"),
             py::arg("src_port"),
             py::arg("dst"),
             py::arg("dst_port"),
             D(hier_block2, disconnect, 1))


        .def("disconnect_all",
             &hier_block2::disconnect_all,
             D(hier_block2, disconnect_all))


        .def("lock", &hier_block2::lock, D(hier_block2, lock))


        .def("unlock", &hier_block2::unlock, D(hier_block2, unlock))


        .def("max_output_buffer",
             &hier_block2::max_output_buffer,
             py::arg("port") = 0,
             D(hier_block2, max_output_buffer))
        .def("set_max_output_buffer",
             (void (hier_block2::*)(int)) & hier_block2::set_max_output_buffer,
             py::arg("max_output_buffer"),
             D(hier_block2, set_max_output_buffer, 0))
        .def("set_max_output_buffer",
             (void (hier_block2::*)(size_t, int)) & hier_block2::set_max_output_buffer,
             py::arg("port"),
             py::arg("max_output_buffer"),
             D(hier_block2, set_max_output_buffer, 1))
        .def("min_output_buffer",
             &hier_block2::min_output_buffer,
             py::arg("port") = 0,
             D(hier_block2, min_output_buffer))
        .def("set_min_output_buffer",
             (void (hier_block2::*)(int)) & hier_block2::set_min_output_buffer,
             py::arg("min_output_buffer"),
             D(hier_block2, set_min_output_buffer, 0))
        .def("set_min_output_buffer",
             (void (hier_block2::*)(size_t, int)) & hier_block2::set_min_output_buffer,
             py::arg("port"),
             py::arg("min_output_buffer"),
             D(hier_block2, set_min_output_buffer, 1))
        // .def("flatten",&hier_block2::flatten)
        .def("to_hier_block2",
             &hier_block2::to_hier_block2,
             D(hier_block2, to_hier_block2))

        .def("has_msg_port",
             &hier_block2::has_msg_port,
             py::arg("which_port"),
             D(hier_block2, has_msg_port))
        .def("message_port_is_hier",
             &hier_block2::message_port_is_hier,
             py::arg("port_id"),
             D(hier_block2, message_port_is_hier))
        .def("message_port_is_hier_in",
             &hier_block2::message_port_is_hier_in,
             py::arg("port_id"),
             D(hier_block2, message_port_is_hier_in))
        .def("message_port_is_hier_out",
             &hier_block2::message_port_is_hier_out,
             py::arg("port_id"),
             D(hier_block2, message_port_is_hier_out))


        .def("primitive_message_port_register_hier_in",
             &hier_block2::message_port_register_hier_in,
             py::arg("port_id"),
             D(hier_block2, message_port_register_hier_in))


        .def("primitive_message_port_register_hier_out",
             &hier_block2::message_port_register_hier_out,
             py::arg("port_id"),
             D(hier_block2, message_port_register_hier_out))
        .def("set_processor_affinity",
             &hier_block2::set_processor_affinity,
             py::arg("mask"),
             D(hier_block2, set_processor_affinity))


        .def("unset_processor_affinity",
             &hier_block2::unset_processor_affinity,
             D(hier_block2, unset_processor_affinity))


        .def("processor_affinity",
             &hier_block2::processor_affinity,
             D(hier_block2, processor_affinity))


        .def("set_log_level",
             &hier_block2::set_log_level,
             py::arg("level"),
             D(hier_block2, set_log_level))


        .def("log_level", &hier_block2::log_level, D(hier_block2, log_level))


        .def("all_min_output_buffer_p",
             &hier_block2::all_min_output_buffer_p,
             D(hier_block2, all_min_output_buffer_p))


        .def("all_max_output_buffer_p",
             &hier_block2::all_max_output_buffer_p,
             D(hier_block2, all_max_output_buffer_p))

        ;


    // m.def("make_hier_block2",&gr::make_hier_block2,
    //     py::arg("name"),
    //     py::arg("input_signature"),
    //     py::arg("output_signature")
    // );
    // m.def("dot_graph",&gr::dot_graph,
    //     py::arg("hierblock2")
    // );
    // m.def("cast_to_hier_block2_sptr",&gr::cast_to_hier_block2_sptr,
    //     py::arg("block")
    // );
}
