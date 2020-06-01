include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND video_sdl_python_files
    sink_s_python.cc
    sink_uc_python.cc
    python_bindings.cc)

GR_PYBIND_MAKE_CHECK_HASH(video_sdl 
   ../../.. 
   gr::video_sdl
   "${video_sdl_python_files}")

install(TARGETS video_sdl_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/video_sdl COMPONENT pythonapi)
