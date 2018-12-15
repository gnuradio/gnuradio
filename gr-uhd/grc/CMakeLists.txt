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
# Rules for generating the source and sink yml wrappers
########################################################################
include(GrPython)

macro(GEN_BLOCK_YML _generator _yml_block)
    set(generator ${CMAKE_CURRENT_SOURCE_DIR}/${_generator})
    set(yml_block ${CMAKE_CURRENT_BINARY_DIR}/${_yml_block})
    list(APPEND yml_blocks ${yml_block})
    add_custom_command(
        DEPENDS ${generator} OUTPUT ${yml_block}
        COMMAND ${PYTHON_EXECUTABLE} ${generator} ${yml_block}
    )
endmacro(GEN_BLOCK_YML)

GEN_BLOCK_YML(gen_uhd_usrp_blocks.py    uhd_usrp_source.block.yml)
GEN_BLOCK_YML(gen_uhd_usrp_blocks.py    uhd_usrp_sink.block.yml)

add_custom_target(uhd_grc_yml_blocks ALL DEPENDS ${yml_blocks})

install(FILES
    ${yml_blocks}
    uhd_amsg_source.block.yml
    uhd.tree.yml
    DESTINATION ${GRC_BLOCKS_DIR}
)
