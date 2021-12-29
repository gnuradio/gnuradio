include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(
    APPEND
    fft_python_files
    #ctrlport_probe_psd_python.cc
    fft_shift_python.cc
    fft_v_python.cc
    goertzel_python.cc
    goertzel_fc_python.cc
    window_python.cc
    python_bindings.cc)

gr_pybind_make_check_hash(fft ../../.. gr::fft "${fft_python_files}")

install(
    TARGETS fft_python
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/fft
    COMPONENT pythonapi)
