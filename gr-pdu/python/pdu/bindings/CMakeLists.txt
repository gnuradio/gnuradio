include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND pdu_python_files
    add_system_time_python.cc
    pdu_filter_python.cc
    pdu_remove_python.cc
    pdu_set_python.cc
    pdu_split_python.cc
    pdu_to_tagged_stream_python.cc
    random_pdu_python.cc
    tagged_stream_to_pdu_python.cc
    time_delta_python.cc
    python_bindings.cc)

GR_PYBIND_MAKE_CHECK_HASH(pdu
   ../../..
   gr::pdu
   "${pdu_python_files}")

install(TARGETS pdu_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/pdu COMPONENT pythonapi)
