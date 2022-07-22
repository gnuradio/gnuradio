
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
#include <gnuradio/types.h>
#include <numpy/arrayobject.h>

#include <gnuradio/block_work_io.h>
#include <vector>


namespace py = pybind11;

void bind_tag(py::module&);
void bind_port(py::module&);
void bind_block_work_io(py::module&);
void bind_node(py::module&);
void bind_pyblock_detail(py::module&);
void bind_block(py::module&);
void bind_sync_block(py::module&);
void bind_edge(py::module&);
void bind_graph(py::module&);
void bind_hier_block(py::module&);
void bind_flowgraph(py::module&);
void bind_scheduler(py::module&);
void bind_buffer(py::module&);
void bind_vmcircbuf(py::module&);
void bind_constants(py::module&);
void bind_python_block(py::module&);
void bind_buffer_net_zmq(py::module& m);
void bind_runtime(py::module&);
void bind_runtime_proxy(py::module&);
void bind_graph_utils(py::module&);
void bind_rpc_client_interface(py::module&);
#ifdef HAVE_CUDA
void bind_buffer_cuda(py::module&);
void bind_buffer_cuda_pinned(py::module&);
void bind_buffer_cuda_sm(py::module&);
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
    py::module::import("pmtf");

    bind_tag(m);
    bind_block_work_io(m);
    bind_port(m);
    bind_node(m);
    bind_pyblock_detail(m);
    bind_block(m);
    bind_sync_block(m);
    bind_edge(m);
    bind_graph(m);
    bind_hier_block(m);
    bind_flowgraph(m);
    bind_scheduler(m);
    bind_buffer(m);
    bind_buffer_net_zmq(m);
    bind_vmcircbuf(m);
    bind_constants(m);
    bind_python_block(m);
    bind_runtime(m);
    bind_runtime_proxy(m);
    bind_graph_utils(m);
    bind_rpc_client_interface(m);
#ifdef HAVE_CUDA
    bind_buffer_cuda(m);
    bind_buffer_cuda_pinned(m);
    bind_buffer_cuda_sm(m);
#endif

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
