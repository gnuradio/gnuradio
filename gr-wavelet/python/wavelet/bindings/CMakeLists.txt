include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND wavelet_python_files
    squash_ff_python.cc
    wavelet_ff_python.cc
    wvps_ff_python.cc
    python_bindings.cc)

GR_PYBIND_MAKE_CHECK_HASH(wavelet 
   ../../.. 
   gr::wavelet
   "${wavelet_python_files}")

install(TARGETS wavelet_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/wavelet COMPONENT pythonapi)
