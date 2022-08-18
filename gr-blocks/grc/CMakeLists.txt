# Copyright 2012-2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
file(GLOB yml_files "*.yml")

# Force out the controlport GRC blocks if we've disabled it.
if(NOT ENABLE_GR_CTRLPORT)
    list(REMOVE_ITEM yml_files ${CMAKE_CURRENT_SOURCE_DIR}/ctrlport_probe_c.block.yml
         ${CMAKE_CURRENT_SOURCE_DIR}/ctrlport_probe2_c.block.yml)
endif(NOT ENABLE_GR_CTRLPORT)

if(NOT SNDFILE_FOUND)
    list(REMOVE_ITEM yml_files ${CMAKE_CURRENT_SOURCE_DIR}/blocks_wavfile_sink.block.yml
         ${CMAKE_CURRENT_SOURCE_DIR}/blocks_wavfile_source.block.yml)
endif()

install(FILES ${yml_files} DESTINATION ${GRC_BLOCKS_DIR})
