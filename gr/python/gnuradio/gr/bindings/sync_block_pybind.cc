
#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/sync_block.h>
// pydoc.h is automatically generated in the build directory
// #include <basic_block_pydoc.h>

void bind_sync_block(py::module& m)
{
    using sync_block = ::gr::sync_block;

    py::class_<sync_block, gr::block, gr::node, std::shared_ptr<sync_block>>(
        m, "sync_block");
}
