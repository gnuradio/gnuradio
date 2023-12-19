# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
file(GLOB yml_files "*.yml")

macro(REPLACE_IN_FILE _yml_block match replace)
    set(yml_block_src "${CMAKE_CURRENT_SOURCE_DIR}/${_yml_block}")
    set(yml_block "${CMAKE_CURRENT_BINARY_DIR}/${_yml_block}")

    list(REMOVE_ITEM yml_files "${yml_block_src}")
    file(READ "${yml_block_src}" yml_block_src_text)
    string(REPLACE "${match}" "${replace}" yml_block_text "${yml_block_src_text}")
    file(WRITE "${yml_block}" "${yml_block_text}")

    list(APPEND generated_yml_files "${yml_block}")
endmacro()

install(FILES ${yml_files} ${generated_yml_files} DESTINATION "${GRC_BLOCKS_DIR}")
