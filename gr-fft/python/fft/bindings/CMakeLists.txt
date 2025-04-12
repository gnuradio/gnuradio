include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(
    APPEND
    fft_python_files
    fft_shift_python.cc
    fft_v_python.cc
    goertzel_python.cc
    goertzel_fc_python.cc
    window_python.cc
    python_bindings.cc)

if(ENABLE_GR_CTRLPORT)
    list(APPEND fft_python_files
    ctrlport_probe_psd_python.cc
        )
endif()

gr_pybind_make_check_hash(fft ../../.. gr::fft "${fft_python_files}")

if(ENABLE_GR_CTRLPORT)
    target_compile_definitions(fft_python PUBLIC GR_HAVE_CTRLPORT)
endif()


install(
    TARGETS fft_python
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/fft
    COMPONENT pythonapi)
