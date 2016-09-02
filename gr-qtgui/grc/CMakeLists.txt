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


if(DESIRED_QT_VERSION EQUAL 4)
    foreach(xml_block_src ${xml_files})
        get_filename_component(xml_block "${xml_block_src}" NAME)
        REPLACE_IN_FILE("${xml_block}" "PyQt5" "PyQt4")
    endforeach()
endif()

install(
    FILES ${xml_files} ${generated_xml_files}
    DESTINATION "${GRC_BLOCKS_DIR}"
)
