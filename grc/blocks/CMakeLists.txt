# Copyright 2011,2018 Free Software Foundation, Inc.
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

macro(REPLACE_IN_FILE _yml_block match replace)
    set(yml_block_src "${CMAKE_CURRENT_SOURCE_DIR}/${_yml_block}")
    set(yml_block     "${CMAKE_CURRENT_BINARY_DIR}/${_yml_block}")

    list(REMOVE_ITEM yml_files "${yml_block_src}")
    file(READ "${yml_block_src}" yml_block_src_text)
    string(REPLACE "${match}" "${replace}"
           yml_block_text "${yml_block_src_text}")
    file(WRITE "${yml_block}" "${yml_block_text}")

    list(APPEND generated_yml_files "${yml_block}")
endmacro()

macro(GEN_BLOCK_YML _generator _yml_block)
    set(generator ${CMAKE_CURRENT_SOURCE_DIR}/${_generator})
    set(yml_block ${CMAKE_CURRENT_BINARY_DIR}/${_yml_block})
    list(APPEND generated_yml_files ${yml_block})
    add_custom_command(
        DEPENDS ${generator} OUTPUT ${yml_block}
        COMMAND ${PYTHON_EXECUTABLE} ${generator} ${yml_block}
    )
endmacro()

GEN_BLOCK_YML(variable_struct.block.yml.py variable_struct.block.yml)

add_custom_target(grc_generated_yml ALL DEPENDS ${generated_yml_files})

install(
    FILES ${yml_files} ${generated_yml_files}
    DESTINATION ${GRC_BLOCKS_DIR}
)
