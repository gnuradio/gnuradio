# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
file(GLOB yml_files "*.yml")

# Without GSL, we don't build some of the LDPC work, so we can't use
# it in grc
if(NOT GSL_FOUND)
    list(
        REMOVE_ITEM
        yml_files
        ${CMAKE_CURRENT_SOURCE_DIR}/variable_ldpc_bit_flip_decoder.block.yml
        ${CMAKE_CURRENT_SOURCE_DIR}/variable_ldpc_G_matrix_object.block.yml
        ${CMAKE_CURRENT_SOURCE_DIR}/variable_ldpc_H_matrix_object.block.yml
        ${CMAKE_CURRENT_SOURCE_DIR}/variable_ldpc_encoder_G.block.yml
        ${CMAKE_CURRENT_SOURCE_DIR}/variable_ldpc_encoder_H.block.yml)
endif(NOT GSL_FOUND)
install(FILES ${yml_files} DESTINATION ${GRC_BLOCKS_DIR})
