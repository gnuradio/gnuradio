
#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/port.h>
// pydoc.h is automatically generated in the build directory
// #include <basic_block_pydoc.h>

void bind_port(py::module& m)
{
    using port_base = ::gr::port_base;
    using port_f = ::gr::port<float>;
    using port_c = ::gr::port<gr_complex>;
    using port_s = ::gr::port<int16_t>;
    using port_i = ::gr::port<int32_t>;
    using port_b = ::gr::port<uint8_t>;

    using message_port = ::gr::message_port;
    using untyped_port = ::gr::untyped_port;

    py::class_<gr::port_interface, std::unique_ptr<gr::port_interface>>(m,
                                                                        "port_interface");

    py::class_<port_base, gr::port_interface, std::unique_ptr<port_base>>(m, "port_base")
        .def("format_descriptor", &gr::port_base::format_descriptor)
        .def("shape", &gr::port_base::shape)
        .def("index", &gr::port_base::index)
        .def("name", &gr::port_base::name)
        .def("connect", &gr::port_base::connect)
        .def("itemsize", &gr::port_base::itemsize)
        .def("type", &gr::port_base::type);

    py::class_<port_f, port_base, std::unique_ptr<port_f>>(m, "port_f");
    py::class_<port_c, port_base, std::unique_ptr<port_c>>(m, "port_c");
    py::class_<port_s, port_base, std::unique_ptr<port_s>>(m, "port_s");
    py::class_<port_i, port_base, std::unique_ptr<port_i>>(m, "port_i");
    py::class_<port_b, port_base, std::unique_ptr<port_b>>(m, "port_b");

    py::class_<message_port,
               port_base,
               gr::port_interface,
               std::unique_ptr<message_port>>(m, "message_port")
        .def("post", &message_port::post);

    py::class_<untyped_port, port_base, std::unique_ptr<untyped_port>>(m, "untyped_port");

    py::enum_<gr::port_direction_t>(m, "port_direction_t")
        .value("INPUT", gr::port_direction_t::INPUT)
        .value("OUTPUT", gr::port_direction_t::OUTPUT)
        .export_values();

    py::enum_<gr::port_type_t>(m, "port_type_t")
        .value("STREAM", gr::port_type_t::STREAM)
        .value("MESSAGE", gr::port_type_t::MESSAGE)
        .export_values();

    py::implicitly_convertible<gr::port_interface, gr::message_port>();
}
