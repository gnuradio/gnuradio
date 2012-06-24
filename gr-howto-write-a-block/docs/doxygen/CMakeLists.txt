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

########################################################################
# Create the doxygen configuration file
########################################################################
file(TO_NATIVE_PATH ${CMAKE_SOURCE_DIR} top_srcdir)
file(TO_NATIVE_PATH ${CMAKE_BINARY_DIR} top_builddir)
file(TO_NATIVE_PATH ${CMAKE_SOURCE_DIR} abs_top_srcdir)
file(TO_NATIVE_PATH ${CMAKE_BINARY_DIR} abs_top_builddir)

set(HAVE_DOT ${DOXYGEN_DOT_FOUND})
set(enable_html_docs YES)
set(enable_latex_docs NO)
set(enable_xml_docs YES)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
@ONLY)

set(BUILT_DIRS ${CMAKE_CURRENT_BINARY_DIR}/xml ${CMAKE_CURRENT_BINARY_DIR}/html)

########################################################################
# Make and install doxygen docs
########################################################################
add_custom_command(
    OUTPUT ${BUILT_DIRS}
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating documentation with doxygen"
)

add_custom_target(doxygen_target ALL DEPENDS ${BUILT_DIRS})

install(DIRECTORY ${BUILT_DIRS} DESTINATION ${GR_PKG_DOC_DIR})
