include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND audio_python_files
     # osx_impl_python.cc
     sink_python.cc source_python.cc python_bindings.cc)

gr_pybind_make_check_hash(audio ../../.. gr::audio "${audio_python_files}")

install(
    TARGETS audio_python
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/audio
    COMPONENT pythonapi)
