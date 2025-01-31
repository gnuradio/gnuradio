include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(
    APPEND
    uhd_python_files
    amsg_source_python.cc
    uhd_types_python.cc
    usrp_block_python.cc
    usrp_sink_python.cc
    usrp_source_python.cc
    python_bindings.cc)

if(ENABLE_UHD_RFNOC)
    add_definitions(-DGR_ENABLE_UHD_RFNOC)
    list(
        APPEND
        uhd_python_files
        rfnoc_block_generic_python.cc
        rfnoc_block_python.cc
        rfnoc_ddc_python.cc
        rfnoc_duc_python.cc
        rfnoc_fir_filter_python.cc
        rfnoc_fft_python.cc
        rfnoc_graph_python.cc
        rfnoc_replay_python.cc
        rfnoc_rx_radio_python.cc
        rfnoc_rx_streamer_python.cc
        rfnoc_siggen_python.cc
        rfnoc_tx_radio_python.cc
        rfnoc_tx_streamer_python.cc
        rfnoc_window_python.cc)
endif()

gr_pybind_make(uhd ../../.. gr::uhd "${uhd_python_files}")

install(
    TARGETS uhd_python
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/uhd
    COMPONENT pythonapi)
