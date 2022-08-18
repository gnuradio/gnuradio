# Copyright 2011,2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Build executable
########################################################################
add_executable(tags_demo tags_demo.cc)
target_link_libraries(tags_demo gnuradio-uhd)

install(TARGETS tags_demo DESTINATION ${GR_PKG_UHD_EXAMPLES_DIR})
