
#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/scheduler.h>
// pydoc.h is automatically generated in the build directory
// #include <basic_block_pydoc.h>

void bind_scheduler(py::module& m)
{
    using scheduler = ::gr::scheduler;

    py::class_<scheduler, std::shared_ptr<scheduler>>(m, "scheduler");
}
