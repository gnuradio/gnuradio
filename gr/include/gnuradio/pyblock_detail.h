#pragma once

#include <gnuradio/api.h>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h> // must be first
#include <pybind11/stl.h>
namespace py = pybind11;

namespace gr {
struct GR_RUNTIME_API pyblock_detail {
    py::handle d_py_handle = nullptr;
    pyblock_detail(py::handle handle) { d_py_handle = handle; }
    py::handle handle() { return d_py_handle; }
};
} // namespace gr