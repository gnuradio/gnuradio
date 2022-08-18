include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(
    APPEND
    iio_python_files
    attr_sink_python.cc
    attr_source_python.cc
    attr_updater_python.cc
    dds_control_python.cc
    device_sink_python.cc
    device_source_python.cc
    iio_types_python.cc
    python_bindings.cc)

if(${libad9361_FOUND})
    list(
        APPEND
        iio_python_files
        fmcomms2_sink_python.cc
        fmcomms2_source_python.cc
        # fmcomms5_sink_f32c_python.cc
        # fmcomms5_sink_python.cc
        # fmcomms5_source_f32c_python.cc
        # fmcomms5_source_python.cc
        pluto_utils_python.cc)
endif(${libad9361_FOUND})

gr_pybind_make_check_hash(iio ../../.. gr::iio "${iio_python_files}")

install(
    TARGETS iio_python
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/iio
    COMPONENT pythonapi)
