
#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/node.h>
// pydoc.h is automatically generated in the build directory
// #include <basic_block_pydoc.h>

void bind_node(py::module& m)
{
    using node = ::gr::node;

    // using port_base = ::gr::port_base;
    using port_f = ::gr::port<float>;
    using port_c = ::gr::port<gr_complex>;
    using port_s = ::gr::port<int16_t>;
    using port_i = ::gr::port<int32_t>;
    using port_b = ::gr::port<uint8_t>;

    using message_port = ::gr::message_port;
    using untyped_port = ::gr::untyped_port;

    py::class_<node, std::shared_ptr<node>>(m, "node")

        .def("name", &node::name)
        .def("alias", &node::alias)
        // .def("add_port",
        //      [](node& self, gr::port_uptr& pt) {
        //          self.add_port(std::move(pt));
        //      })
        .def("get_port",
             py::overload_cast<unsigned int, gr::port_type_t, gr::port_direction_t>(
                 &gr::node::get_port),
             py::return_value_policy::reference)
        .def("get_port",
             py::overload_cast<const std::string&>(&gr::node::get_port),
             py::return_value_policy::reference)
        .def("get_message_port",
             &node::get_message_port,
             py::return_value_policy::reference)
        .def("set_rpc", &node::set_rpc)
        .def("rpc_client", &node::rpc_client)
        .def("rpc_name", &node::rpc_name)
        .def("input_ports", &node::input_ports)
        .def("output_ports", &node::output_ports)
        .def(
            "add_port_f",
            [](node& self,
               const std::string& name,
               gr::port_direction_t dir,
               std::vector<size_t> shape = { 1 },
               bool optional = false,
               size_t multiplicity = 1) {
                self.add_port(port_f::make(name, dir, shape, optional, multiplicity));
            },
            py::arg("name"),
            py::arg("direction"),
            py::arg("shape") = std::vector<size_t>{ 1 },
            py::arg("optional") = false,
            py::arg("multiplicity") = 1)
        .def(
            "add_port_c",
            [](node& self,
               const std::string& name,
               gr::port_direction_t dir,
               std::vector<size_t> shape = { 1 },
               bool optional = false,
               size_t multiplicity = 1) {
                self.add_port(port_c::make(name, dir, shape, optional, multiplicity));
            },
            py::arg("name"),
            py::arg("direction"),
            py::arg("shape") = std::vector<size_t>{ 1 },
            py::arg("optional") = false,
            py::arg("multiplicity") = 1)
        .def(
            "add_port_s",
            [](node& self,
               const std::string& name,
               gr::port_direction_t dir,
               std::vector<size_t> shape = { 1 },
               bool optional = false,
               size_t multiplicity = 1) {
                self.add_port(port_s::make(name, dir, shape, optional, multiplicity));
            },
            py::arg("name"),
            py::arg("direction"),
            py::arg("shape") = std::vector<size_t>{ 1 },
            py::arg("optional") = false,
            py::arg("multiplicity") = 1)
        .def(
            "add_port_i",
            [](node& self,
               const std::string& name,
               gr::port_direction_t dir,
               std::vector<size_t> shape = { 1 },
               bool optional = false,
               size_t multiplicity = 1) {
                self.add_port(port_i::make(name, dir, shape, optional, multiplicity));
            },
            py::arg("name"),
            py::arg("direction"),
            py::arg("shape") = std::vector<size_t>{ 1 },
            py::arg("optional") = false,
            py::arg("multiplicity") = 1)
        .def(
            "add_port_b",
            [](node& self,
               const std::string& name,
               gr::port_direction_t dir,
               std::vector<size_t> shape = { 1 },
               bool optional = false,
               size_t multiplicity = 1) {
                self.add_port(port_b::make(name, dir, shape, optional, multiplicity));
            },
            py::arg("name"),
            py::arg("direction"),
            py::arg("shape") = std::vector<size_t>{ 1 },
            py::arg("optional") = false,
            py::arg("multiplicity") = 1)
        .def(
            "add_message_port",
            [](node& self,
               const std::string& name,
               const gr::port_direction_t direction,
               const bool optional = false,
               const int multiplicity = 1) {
                self.add_port(
                    message_port::make(name, direction, optional, multiplicity));
            },
            py::arg("name"),
            py::arg("direction"),
            py::arg("optional") = false,
            py::arg("multiplicity") = 1)
        .def(
            "add_untyped_port",
            [](node& self,
               const std::string& name,
               const gr::port_direction_t direction,
               const size_t itemsize,
               const bool optional = false,
               const int multiplicity) {
                self.add_port(untyped_port::make(
                    name, direction, itemsize, optional, multiplicity));
            },
            py::arg("name"),
            py::arg("direction"),
            py::arg("itemsize"),
            py::arg("optional") = false,
            py::arg("multiplicity") = 1)


        ;
}
