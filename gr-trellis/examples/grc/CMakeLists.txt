# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

## Configure GRC examples to point to the right fsm_files directory
set(FSM_FILE_INSTALL_DIR
    "${CMAKE_INSTALL_PREFIX}/${GR_PKG_TRELLIS_EXAMPLES_DIR}/fsm_files/")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/tcm.grc"
               "${CMAKE_CURRENT_BINARY_DIR}/tcm.grc")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/viterbi_equalization.grc"
               "${CMAKE_CURRENT_BINARY_DIR}/viterbi_equalization.grc")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/turbo_equalization.grc"
               "${CMAKE_CURRENT_BINARY_DIR}/turbo_equalization.grc")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/sccc.grc"
               "${CMAKE_CURRENT_BINARY_DIR}/sccc.grc")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/pccc.grc"
               "${CMAKE_CURRENT_BINARY_DIR}/pccc.grc")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/interference_cancellation.grc"
               "${CMAKE_CURRENT_BINARY_DIR}/interference_cancellation.grc")

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/interference_cancellation.grc
          ${CMAKE_CURRENT_BINARY_DIR}/pccc.grc
          ${CMAKE_CURRENT_BINARY_DIR}/sccc.grc
          ${CMAKE_CURRENT_BINARY_DIR}/tcm.grc
          ${CMAKE_CURRENT_BINARY_DIR}/turbo_equalization.grc
          ${CMAKE_CURRENT_BINARY_DIR}/viterbi_equalization.grc
          readme.txt
    DESTINATION ${GR_PKG_TRELLIS_EXAMPLES_DIR})
