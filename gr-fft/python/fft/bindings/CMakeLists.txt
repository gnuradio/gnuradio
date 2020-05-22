include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND fft_python_files
    #ctrlport_probe_psd_python.cc
    fft_python.cc
    fft_shift_python.cc
    fft_vcc_python.cc
    fft_vfc_python.cc
    goertzel_python.cc
    goertzel_fc_python.cc
    window_python.cc
    python_bindings.cc)

GR_PYBIND_MAKE_CHECK_HASH(fft 
   ../../.. 
   gr::fft
   "${fft_python_files}")

install(TARGETS fft_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/fft COMPONENT pythonapi)
