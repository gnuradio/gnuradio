include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND channels_python_files
    cfo_model_python.cc
    channel_model_python.cc
    channel_model2_python.cc
    dynamic_channel_model_python.cc
    fading_model_python.cc
    selective_fading_model_python.cc
    selective_fading_model2_python.cc
    sro_model_python.cc
    python_bindings.cc)

GR_PYBIND_MAKE(channels 
   ../../.. 
   gr::channels
   "${channels_python_files}")

install(TARGETS channels_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/channels COMPONENT pythonapi)
