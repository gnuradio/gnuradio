include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND uhd_python_files
  uhd_types_python.cc
  amsg_source_python.cc
  usrp_block_python.cc
  usrp_sink_python.cc
  usrp_source_python.cc
  python_bindings.cc
  )

if(ENABLE_UHD_RFNOC)
  add_definitions(-DGR_ENABLE_UHD_RFNOC)
  list(APPEND uhd_python_files
    rfnoc_graph_python.cc
    rfnoc_rx_streamer_python.cc
    rfnoc_tx_streamer_python.cc
    )
endif()

GR_PYBIND_MAKE(uhd
  ../../..
  gr::uhd
  "${uhd_python_files}")

install(TARGETS uhd_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/uhd COMPONENT pythonapi)
