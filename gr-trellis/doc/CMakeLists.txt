# Copyright 2011 Free Software Foundation, Inc.
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

include(GrPython)

########################################################################
# Generate HTML doc with xmlto
########################################################################
find_program(XMLTO_EXECUTABLE xmlto)

if(XMLTO_EXECUTABLE)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gr-trellis.html
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gr-trellis.xml
    COMMAND ${XMLTO_EXECUTABLE} html-nochunks
        ${CMAKE_CURRENT_SOURCE_DIR}/gr-trellis.xml
)
add_custom_target(gr_trellis_html ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/gr-trellis.html)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gr-trellis.html
    DESTINATION ${GR_PKG_DOC_DIR}/html
    COMPONENT "trellis_docs"

)
endif(XMLTO_EXECUTABLE)

########################################################################
# Generate xml doc
########################################################################
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/test_tcm.py.xml
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test_tcm.py
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/make_numbered_listing.py
    COMMAND ${PYTHON_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/make_numbered_listing.py
        ${CMAKE_CURRENT_SOURCE_DIR}/test_tcm.py
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/test_viterbi_equalization1.py.xml
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test_viterbi_equalization1.py
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/make_numbered_listing.py
    COMMAND ${PYTHON_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/make_numbered_listing.py
        ${CMAKE_CURRENT_SOURCE_DIR}/test_viterbi_equalization1.py
)

add_custom_target(gr_trellis_xml ALL DEPENDS
    ${CMAKE_CURRENT_BINARY_DIR}/test_tcm.py.xml
    ${CMAKE_CURRENT_BINARY_DIR}/test_viterbi_equalization1.py.xml
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/test_tcm.py.xml
    ${CMAKE_CURRENT_BINARY_DIR}/test_viterbi_equalization1.py.xml
    DESTINATION ${GR_PKG_DOC_DIR}/xml
    COMPONENT "trellis_docs"
)
