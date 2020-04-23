include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND network_python_files
    # packet_headers_python.cc
    tcp_sink_python.cc
    # udp_header_types_python.cc
    udp_sink_python.cc
    udp_source_python.cc
    python_bindings.cc)

GR_PYBIND_MAKE(network 
   ../../.. 
   gr::network
   "${network_python_files}")

install(TARGETS network_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/network COMPONENT pythonapi)
