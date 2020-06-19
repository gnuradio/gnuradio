include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND uhd_python_files
    uhd_types_python.cc
    amsg_source_python.cc
    usrp_block_python.cc
    usrp_sink_python.cc
    usrp_source_python.cc
    python_bindings.cc)

GR_PYBIND_MAKE(uhd 
   ../../.. 
   gr::uhd
   "${uhd_python_files}")

install(TARGETS uhd_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/uhd COMPONENT pythonapi)
