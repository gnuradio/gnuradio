# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

## Configure GRC examples to point to the right fsm_files directory
set(FSM_FILE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${GR_PKG_TRELLIS_EXAMPLES_DIR}/fsm_files/")
configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/pccc.grc"
    "${CMAKE_CURRENT_BINARY_DIR}/pccc.grc"
)

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/pccc1.grc"
    "${CMAKE_CURRENT_BINARY_DIR}/pccc1.grc"
)

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/sccc.grc"
    "${CMAKE_CURRENT_BINARY_DIR}/sccc.grc"
)

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/sccc1.grc"
    "${CMAKE_CURRENT_BINARY_DIR}/sccc1.grc"
)

install(
    FILES
    interference_cancellation.grc
    ${CMAKE_CURRENT_BINARY_DIR}/pccc.grc
    ${CMAKE_CURRENT_BINARY_DIR}/pccc1.grc
    ${CMAKE_CURRENT_BINARY_DIR}/sccc1.grc
    ${CMAKE_CURRENT_BINARY_DIR}/sccc.grc
    readme.txt
    DESTINATION ${GR_PKG_TRELLIS_EXAMPLES_DIR}
    COMPONENT "trellis-examples"
)
