# Copyright 2010-2011,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup executables
########################################################################
add_executable(gnuradio-config-info gnuradio-config-info.cc)
target_link_libraries(gnuradio-config-info gnuradio-runtime Boost::program_options)
install(TARGETS gnuradio-config-info DESTINATION ${GR_RUNTIME_DIR})
configure_file(gnuradio.bat ${CMAKE_BINARY_DIR}/gnuradio.bat)
configure_file(setup_gr.py ${CMAKE_BINARY_DIR}/setup_gr.py)
install(PROGRAMS ${CMAKE_BINARY_DIR}/gnuradio.bat ${CMAKE_BINARY_DIR}/setup_gr.py DESTINATION ${GR_RUNTIME_DIR})
