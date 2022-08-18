include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(
    APPEND
    pdu_python_files
    add_system_time_python.cc
    pdu_filter_python.cc
    pdu_remove_python.cc
    pdu_set_python.cc
    pdu_split_python.cc
    pdu_to_stream_python.cc
    pdu_to_tagged_stream_python.cc
    random_pdu_python.cc
    tags_to_pdu_python.cc
    tagged_stream_to_pdu_python.cc
    take_skip_to_pdu_python.cc
    time_delta_python.cc
    python_bindings.cc)

gr_pybind_make_check_hash(pdu ../../.. gr::pdu "${pdu_python_files}")

install(
    TARGETS pdu_python
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/pdu
    COMPONENT pythonapi)
