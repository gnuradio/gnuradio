include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND zeromq_python_files
    pub_msg_sink_python.cc
    pub_sink_python.cc
    pull_msg_source_python.cc
    pull_source_python.cc
    push_msg_sink_python.cc
    push_sink_python.cc
    rep_msg_sink_python.cc
    rep_sink_python.cc
    req_msg_source_python.cc
    req_source_python.cc
    sub_msg_source_python.cc
    sub_source_python.cc
    python_bindings.cc)

GR_PYBIND_MAKE_CHECK_HASH(zeromq 
   ../../.. 
   gr::zeromq
   "${zeromq_python_files}")

install(TARGETS zeromq_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/zeromq COMPONENT pythonapi)
