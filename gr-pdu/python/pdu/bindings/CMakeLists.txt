include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND pdu_python_files
    pdu_split_python.cc
    pdu_python.cc
    python_bindings.cc)

GR_PYBIND_MAKE_CHECK_HASH(pdu 
   ../../.. 
   gr::pdu
   "${pdu_python_files}")

install(TARGETS pdu_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/pdu COMPONENT pythonapi)
