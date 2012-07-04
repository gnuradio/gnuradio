# Copyright 2010-2012 Free Software Foundation, Inc.
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
# This file included, use CMake directory variables
########################################################################
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

########################################################################
# Generate serial tags header file
########################################################################

get_filename_component(PMT_SERIAL_TAGS_H
    ${CMAKE_CURRENT_BINARY_DIR}/../../include/gruel/pmt_serial_tags.h ABSOLUTE
)

add_custom_command(
    OUTPUT ${PMT_SERIAL_TAGS_H}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gen-serial-tags.py
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pmt-serial-tags.scm
    COMMAND ${PYTHON_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/gen-serial-tags.py
        ${CMAKE_CURRENT_SOURCE_DIR}/pmt-serial-tags.scm
        ${PMT_SERIAL_TAGS_H}
)

install(
    FILES ${PMT_SERIAL_TAGS_H}
    DESTINATION ${GR_INCLUDE_DIR}/gruel
    COMPONENT "gruel_devel"
)

include(AddFileDependencies)
ADD_FILE_DEPENDENCIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/pmt_serialize.cc
    ${PMT_SERIAL_TAGS_H}
)

add_custom_target(pmt_generated DEPENDS ${PMT_SERIAL_TAGS_H})

########################################################################
# Generate other pmt stuff
########################################################################
add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/pmt_unv_int.h
        ${CMAKE_CURRENT_BINARY_DIR}/qa_pmt_unv.h
        ${CMAKE_CURRENT_BINARY_DIR}/pmt_unv.cc
        ${CMAKE_CURRENT_BINARY_DIR}/qa_pmt_unv.cc
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/generate_unv.py
        ${CMAKE_CURRENT_SOURCE_DIR}/unv_template.h.t
        ${CMAKE_CURRENT_SOURCE_DIR}/unv_template.cc.t
        ${CMAKE_CURRENT_SOURCE_DIR}/unv_qa_template.cc.t
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B} -c
    \"import os,sys\;srcdir='${CMAKE_CURRENT_SOURCE_DIR}'\;sys.path.append(srcdir)\;os.environ['srcdir']=srcdir\;from generate_unv import main\;main()\"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

list(APPEND gruel_sources
    ${CMAKE_CURRENT_BINARY_DIR}/pmt_unv.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/pmt.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/pmt_io.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/pmt_pool.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/pmt_serialize.cc
)

list(APPEND test_gruel_sources
    ${CMAKE_CURRENT_BINARY_DIR}/qa_pmt_unv.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_pmt_prims.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_pmt.cc
)
