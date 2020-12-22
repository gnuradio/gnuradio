include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND message_python_files
    message_counter_python.cc
    message_emitter_python.cc
    port_names_python.cc
    python_bindings.cc)

GR_PYBIND_MAKE_CHECK_HASH(message 
   ../../.. 
   gr::message
   "${message_python_files}")

install(TARGETS message_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/message COMPONENT pythonapi)
