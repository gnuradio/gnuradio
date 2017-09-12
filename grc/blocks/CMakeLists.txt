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
include(GrPython)

file(GLOB yml_files "*.yml")
file(GLOB xml_files "*.xml")

macro(REPLACE_IN_FILE _xml_block match replace)
    set(xml_block_src "${CMAKE_CURRENT_SOURCE_DIR}/${_xml_block}")
    set(xml_block     "${CMAKE_CURRENT_BINARY_DIR}/${_xml_block}")

    list(REMOVE_ITEM xml_files "${xml_block_src}")
    file(READ "${xml_block_src}" xml_block_src_text)
    string(REPLACE "${match}" "${replace}"
           xml_block_text "${xml_block_src_text}")
    file(WRITE "${xml_block}" "${xml_block_text}")

    list(APPEND generated_xml_files "${xml_block}")
endmacro()

macro(GEN_BLOCK_XML _generator _xml_block)
    set(generator ${CMAKE_CURRENT_SOURCE_DIR}/${_generator})
    set(xml_block ${CMAKE_CURRENT_BINARY_DIR}/${_xml_block})
    list(APPEND generated_xml_files ${xml_block})
    add_custom_command(
        DEPENDS ${generator} OUTPUT ${xml_block}
        COMMAND ${PYTHON_EXECUTABLE} ${generator} ${xml_block}
    )
endmacro()

GEN_BLOCK_XML(variable_struct.xml.py variable_struct.xml)

if(DESIRED_QT_VERSION EQUAL 4)
    REPLACE_IN_FILE(options.xml PyQt5 PyQt4)
endif()

add_custom_target(grc_generated_xml ALL DEPENDS ${generated_xml_files})

install(
    FILES ${yml_files} ${xml_files} ${generated_xml_files}
    DESTINATION ${GRC_BLOCKS_DIR}
)
