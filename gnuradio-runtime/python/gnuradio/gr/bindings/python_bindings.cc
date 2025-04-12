
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <pybind11/pybind11.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <gnuradio/gr_complex.h>
#include <numpy/arrayobject.h>

namespace py = pybind11;

void bind_messages_msg_accepter_msgq(py::module&);
void bind_messages_msg_accepter(py::module&);
void bind_messages_msg_passing(py::module&);
void bind_messages_msg_producer(py::module&);
void bind_messages_msg_queue(py::module&);

// void bind_attributes(py::module&);
void bind_basic_block(py::module&);
void bind_block(py::module&);
void bind_block_gateway(py::module&);
// void bind_block_registry(py::module&);
void bind_buffer(py::module&);
void bind_buffer_type(py::module& m);
void bind_constants(py::module&);
void bind_dictionary_logger_backend(py::module&);
void bind_endianness(py::module&);
void bind_expj(py::module&);
void bind_flowgraph(py::module&);
void bind_fxpt(py::module&);
void bind_fxpt_nco(py::module&);
void bind_fxpt_vco(py::module&);
// void bind_gr_complex(py::module&);
void bind_hier_block2(py::module&);
void bind_high_res_timer(py::module&);
void bind_io_signature(py::module&);
void bind_logger(py::module&);
void bind_math(py::module&);
void bind_message(py::module&);
void bind_msg_accepter(py::module&);
// void bind_msg_accepter_msgq(py::module&);
// void bind_msg_passing(py::module&);
// void bind_msg_producer(py::module&);
void bind_msg_queue(py::module&);
// void bind_misc(py::module&);;
void bind_msg_handler(py::module&);
void bind_msg_queue(py::module&);
void bind_nco(py::module&);
void bind_pdu(py::module&);
void bind_prefs(py::module&);
// void bind_pycallback_object(py::module&);
void bind_random(py::module&);
void bind_realtime(py::module&);
void bind_runtime_types(py::module&);
void bind_sincos(py::module&);
void bind_sptr_magic(py::module&);
void bind_sync_block(py::module&);
void bind_sync_decimator(py::module&);
void bind_sync_interpolator(py::module&);
void bind_sys_paths(py::module&);
void bind_tagged_stream_block(py::module&);
void bind_tags(py::module&);
// void bind_thread(py::module&);
// void bind_thread_body_wrapper(py::module&);
// void bind_thread_group(py::module&);
void bind_top_block(py::module&);
// void bind_types(py::module&);
// void bind_unittests(py::module&);
// void bind_xoroshiro128p(py::module&);
//
#ifdef GR_HAVE_CTRLPORT
// void bind_rpcbufferedget(py::module&);
void bind_rpccallbackregister_base(py::module&);
void bind_rpcmanager(py::module&);
// void bind_rpcmanager_base(py::module&);
// void bind_rpcpmtconverters_thrift(py::module&);
// void bind_rpcregisterhelpers(py::module&);
// void bind_rpcserver_aggregator(py::module&);
// void bind_rpcserver_base(py::module&);
// void bind_rpcserver_booter_aggregator(py::module&);
void bind_rpcserver_booter_base(py::module&);
// void bind_rpcserver_booter_thrift(py::module&);
// void bind_rpcserver_selector(py::module&);
// void bind_rpcserver_thrift(py::module&);
// void bind_thrift_application_base(py::module&);
// void bind_thrift_server_template(py::module&);
#endif

// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(gr_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("pmt");


    bind_messages_msg_accepter(m);
    bind_messages_msg_queue(m);
    bind_messages_msg_accepter_msgq(m);
    bind_messages_msg_passing(m);
    bind_messages_msg_producer(m);


    bind_msg_accepter(m);
    bind_msg_handler(m);
    bind_msg_queue(m);

    bind_buffer_type(m);
    bind_io_signature(m);
    // // bind_attributes(m);
    bind_basic_block(m);
    bind_block(m);
    bind_block_gateway(m);
    // // bind_block_registry(m);
    bind_buffer(m);
    bind_constants(m);
    bind_endianness(m);
    bind_expj(m);
    bind_flowgraph(m);
    bind_fxpt(m);
    bind_fxpt_nco(m);
    bind_fxpt_vco(m);
    // // bind_gr_complex(m);
    bind_hier_block2(m);
    bind_high_res_timer(m);

    bind_logger(m);
    bind_math(m);
    bind_message(m);

    bind_dictionary_logger_backend(m);
    // // bind_msg_accepter_msgq(m);
    // // bind_msg_passing(m);
    // // bind_msg_producer(m);

    // // bind_misc(m);


    bind_nco(m);
    bind_pdu(m);
    bind_prefs(m);
    // // bind_pycallback_object(m);
    bind_random(m);
    bind_realtime(m);
#ifdef GR_HAVE_CTRLPORT
    // // bind_rpcbufferedget(m);
    bind_rpccallbackregister_base(m);
    bind_rpcmanager(m);
    // // bind_rpcmanager_base(m);
    // // bind_rpcpmtconverters_thrift(m);
    // // bind_rpcregisterhelpers(m);
    // // bind_rpcserver_aggregator(m);
    // // bind_rpcserver_base(m);
    // // bind_rpcserver_booter_aggregator(m);
    bind_rpcserver_booter_base(m);
    // // bind_rpcserver_booter_thrift(m);
    // // bind_rpcserver_selector(m);
    // // bind_rpcserver_thrift(m);
#endif
    bind_runtime_types(m);
    bind_sincos(m);
    bind_sptr_magic(m);
    bind_sync_block(m);
    bind_sync_decimator(m);
    bind_sync_interpolator(m);
    bind_sys_paths(m);
    bind_tagged_stream_block(m);
    bind_tags(m);
    // // bind_thread(m);
    // // bind_thread_body_wrapper(m);
    // // bind_thread_group(m);
    // // bind_thrift_application_base(m);
    // // bind_thrift_server_template(m);
    bind_top_block(m);
    // // bind_types(m);
    // // bind_unittests(m);
    // // bind_xoroshiro128p(m);


    // TODO: Move into gr_types.hpp
    // %constant int sizeof_char 	= sizeof(char);
    m.attr("sizeof_char") = sizeof(char);
    // %constant int sizeof_short	= sizeof(short);
    m.attr("sizeof_short") = sizeof(short);
    // %constant int sizeof_int	= sizeof(int);
    m.attr("sizeof_int") = sizeof(int);
    // %constant int sizeof_float	= sizeof(float);
    m.attr("sizeof_float") = sizeof(float);
    // %constant int sizeof_double	= sizeof(double);
    m.attr("sizeof_double") = sizeof(double);
    // %constant int sizeof_gr_complex	= sizeof(gr_complex);
    m.attr("sizeof_gr_complex") = sizeof(gr_complex);
}
