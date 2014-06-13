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

configure_file (
    interference_cancellation.grc.in
    interference_cancellation.grc
    )
configure_file (
    pccc1.grc.in
    pccc1.grc
    )
configure_file (
    pccc.grc.in
    pccc.grc
    )
configure_file (
    sccc1.grc.in
    sccc1.grc
    )
configure_file (
    sccc.grc.in
    sccc.grc
    )

install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/interference_cancellation.grc
    ${CMAKE_CURRENT_BINARY_DIR}/pccc1.grc
    ${CMAKE_CURRENT_BINARY_DIR}/pccc.grc
    ${CMAKE_CURRENT_BINARY_DIR}/sccc1.grc
    ${CMAKE_CURRENT_BINARY_DIR}/sccc.grc
    readme.txt
    DESTINATION ${GR_PKG_TRELLIS_EXAMPLES_DIR}
    COMPONENT "trellis-examples"
)
